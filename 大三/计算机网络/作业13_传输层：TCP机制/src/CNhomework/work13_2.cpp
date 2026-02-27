#include<iostream>
#include<thread>
#include<chrono>
#include<random>
#include<mutex>
#include<condition_variable>
#include<queue>
#include<atomic>

class StopAndWaitProtocol
{
private:
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<int> data_queue;
    std::queue<int> ack_queue;
    std::atomic<bool> running{ true };
    std::atomic<int> seq_num{ 0 };
    std::atomic<int> expected_seq{ 0 };
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;
    std::atomic<int> packets_sent{ 0 };
    std::atomic<int> packets_received{ 0 };
    std::atomic<int> acks_sent{ 0 };
    std::atomic<int> acks_received{ 0 };
    std::atomic<int> packets_lost{ 0 };
    std::atomic<int> acks_lost{ 0 };
    std::atomic<int> retransmissions{ 0 };

    static const int MAX_RETRANSMISSIONS = 5;
public:
    StopAndWaitProtocol() : gen(rd()), dis(0.0, 1.0) {}

    void sender(int total_packets, double packet_loss_rate, double ack_loss_rate, int timeout_ms)
    {
        std::cout << "发送方启动，总共发送 " << total_packets << " 个数据包" << std::endl;
        std::cout << "数据包丢失率: " << packet_loss_rate * 100 << "%, ACK丢失率: " << ack_loss_rate * 100 << "%, 超时时间: " << timeout_ms << "ms" << std::endl;

        int current_seq = 0;
        while (current_seq < total_packets && running)
        {
            int retry_count = 0;
            bool ack_received = false;

            while (!ack_received && retry_count <= MAX_RETRANSMISSIONS && running)
            {
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    data_queue.push(current_seq);
                    packets_sent++;
                    if (retry_count == 0) {
                        std::cout << "发送方: 发送数据包 [序列号=" << current_seq << "]" << std::endl;
                    }
                    else {
                        std::cout << "发送方: 重传数据包 [序列号=" << current_seq << ", 重试次数=" << retry_count << "]" << std::endl;
                        retransmissions++;
                    }
                    cv.notify_all();
                }

                auto start_time = std::chrono::steady_clock::now();
                bool timeout_occurred = false;

                while (!ack_received && !timeout_occurred && running)
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    if (cv.wait_for(lock, std::chrono::milliseconds(timeout_ms),[this, current_seq]()
                        {
                            return !ack_queue.empty();
                        }))
                    {
                        int ack_seq = ack_queue.front();
                        ack_queue.pop();

                        if (ack_seq == current_seq)
                        {
                            ack_received = true;
                            acks_received++;
                            std::cout << "发送方: 收到ACK [序列号=" << current_seq << "]" << std::endl;
                            current_seq++;
                        }
                        else if (ack_seq > current_seq)
                        {
                            std::cout << "发送方: 收到未来ACK [序列号=" << ack_seq << "]，当前=" << current_seq << "，跳过当前包" << std::endl;
                            ack_received = true;
                            current_seq = ack_seq;
                        }
                        else
                        {
                            std::cout << "发送方: 收到过时ACK [序列号=" << ack_seq << "]，期望=" << current_seq << "，忽略" << std::endl;
                        }
                    }
                    else
                    {
                        auto current_time = std::chrono::steady_clock::now();
                        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time);

                        if (elapsed.count() >= timeout_ms)
                        {
                            timeout_occurred = true;
                            retry_count++;
                            if (retry_count > MAX_RETRANSMISSIONS) {
                                std::cout << "发送方: 达到最大重传次数，放弃数据包 [序列号=" << current_seq << "]" << std::endl;
                                current_seq++;
                                break;
                            }
                        }
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        if (current_seq >= total_packets) {
            std::cout << "发送方: 所有数据包发送完成！" << std::endl;
        }
        std::cout << "发送方完成，总共发送 " << packets_sent << " 个数据包" << std::endl;
        std::cout << "重传次数: " << retransmissions << std::endl;
    }

    void receiver(double packet_loss_rate, double ack_loss_rate)
    {
        std::cout << "接收方启动" << std::endl;
        while (running)
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this]() {
                return !data_queue.empty() || !running;
                });

            if (!running && data_queue.empty()) {
                break;
            }
            if (!data_queue.empty()) {
                int packet_seq = data_queue.front();
                data_queue.pop();
                lock.unlock();
                if (dis(gen) < packet_loss_rate)
                {
                    packets_lost++;
                    std::cout << "接收方: 数据包丢失 [序列号=" << packet_seq << "]" << std::endl;
                    continue;
                }
                if (packet_seq == expected_seq)
                {
                    packets_received++;
                    expected_seq++;
                    std::cout << "接收方: 接收数据包 [序列号=" << packet_seq << "]，期望下一个=" << expected_seq << std::endl;
                }
                else if (packet_seq < expected_seq)
                {
                    std::cout << "接收方: 收到重复数据包 [序列号=" << packet_seq << "]，期望=" << expected_seq << "，发送重复ACK" << std::endl;
                }
                else
                {
                    std::cout << "接收方: 收到乱序数据包 [序列号=" << packet_seq << "，期望=" << expected_seq << "]，暂时缓存" << std::endl;
                }
                int ack_to_send = expected_seq - 1;
                if (dis(gen) >= ack_loss_rate)
                {
                    std::lock_guard<std::mutex> ack_lock(mtx);
                    ack_queue.push(ack_to_send);
                    acks_sent++;
                    std::cout << "接收方: 发送ACK [序列号=" << ack_to_send << "]" << std::endl;
                    cv.notify_all();
                }
                else
                {
                    acks_lost++;
                    std::cout << "接收方: ACK丢失 [序列号=" << ack_to_send << "]" << std::endl;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        std::cout << "接收方完成，总共接收 " << packets_received << " 个数据包" << std::endl;
    }
    void stop()
    {
        running = false;
        cv.notify_all();
    }
};

int main() {
    StopAndWaitProtocol protocol;

    const int TOTAL_PACKETS = 10;
    const double PACKET_LOSS_RATE = 0.2;
    const double ACK_LOSS_RATE = 0.2;
    const int TIMEOUT_MS = 500;

    std::thread sender_thread([&]() {
        protocol.sender(TOTAL_PACKETS, PACKET_LOSS_RATE, ACK_LOSS_RATE, TIMEOUT_MS);
        });

    std::thread receiver_thread([&]() {
        protocol.receiver(PACKET_LOSS_RATE, ACK_LOSS_RATE);
        });

    sender_thread.join();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    protocol.stop();
    receiver_thread.join();
    return 0;
}