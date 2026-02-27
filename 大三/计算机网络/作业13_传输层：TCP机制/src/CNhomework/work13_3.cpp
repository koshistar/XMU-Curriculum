#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>
#include <iomanip>
#include <queue>
#include <map>

class SlidingWindowProtocol
{
private:
    struct WindowState
    {
        int base;
        int next_seq;
        int window_size;
        std::vector<bool> ack_received;
        std::vector<bool> sent;
        std::map<int, std::chrono::steady_clock::time_point> send_times; // 记录发送时间用于超时检测
        WindowState(int size) : base(0), next_seq(0), window_size(size)
        {
            ack_received.resize(size * 3, false);
            sent.resize(size * 3, false);
        }
    };

    std::mutex mtx;
    std::condition_variable cv;
    std::queue<int> data_queue;
    std::queue<int> ack_queue;
    std::atomic<bool> running{ true };
    WindowState sender_window;
    WindowState receiver_window;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;
    std::atomic<int> total_packets{ 0 };
    std::atomic<int> packets_sent{ 0 };
    std::atomic<int> packets_received{ 0 };
    std::atomic<int> acks_sent{ 0 };
    std::atomic<int> acks_received{ 0 };
    std::atomic<int> packets_lost{ 0 };
    std::atomic<int> acks_lost{ 0 };
    std::atomic<int> retransmissions{ 0 };

    static const int TIMEOUT_MS = 500;
    static const int MAX_RETRANSMISSIONS = 5;

public:
    SlidingWindowProtocol(int window_size) : sender_window(window_size), receiver_window(window_size), gen(rd()), dis(0.0, 1.0) {}

    void sender(int total_packets_count, double packet_loss_rate, double ack_loss_rate)
    {
        total_packets = total_packets_count;
        std::cout << "发送方启动，窗口大小: " << sender_window.window_size << std::endl;

        while (sender_window.base < total_packets && running)
        {
            std::unique_lock<std::mutex> lock(mtx);

            while (sender_window.next_seq < sender_window.base + sender_window.window_size && sender_window.next_seq < total_packets)
            {
                if (!sender_window.sent[sender_window.next_seq])
                {
                    if (dis(gen) >= packet_loss_rate)
                    {
                        data_queue.push(sender_window.next_seq);
                        sender_window.sent[sender_window.next_seq] = true;
                        sender_window.send_times[sender_window.next_seq] = std::chrono::steady_clock::now();
                        packets_sent++;
                        std::cout << "发送方: 发送数据包 [seq=" << sender_window.next_seq << "]" << std::endl;
                    }
                    else
                    {
                        packets_lost++;
                        std::cout << "发送方: 数据包丢失 [seq=" << sender_window.next_seq << "]" << std::endl;
                    }
                }
                sender_window.next_seq++;
            }
            auto now = std::chrono::steady_clock::now();
            for (int seq = sender_window.base; seq < sender_window.next_seq; seq++)
            {
                if (sender_window.sent[seq] && !sender_window.ack_received[seq])
                {
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - sender_window.send_times[seq]);
                    if (elapsed.count() > TIMEOUT_MS)
                    {
                        if (dis(gen) >= packet_loss_rate)
                        {
                            data_queue.push(seq);
                            sender_window.send_times[seq] = now;
                            retransmissions++;
                            std::cout << "发送方: 超时重传数据包 [seq=" << seq << "]" << std::endl;
                        }
                        else {
                            packets_lost++;
                            std::cout << "发送方: 重传数据包丢失 [seq=" << seq << "]" << std::endl;
                        }
                    }
                }
            }
            while (!ack_queue.empty())
            {
                int ack_seq = ack_queue.front();
                ack_queue.pop();

                if (ack_seq >= sender_window.base && ack_seq < sender_window.base + sender_window.window_size * 2)
                {
                    if (!sender_window.ack_received[ack_seq])
                    {
                        sender_window.ack_received[ack_seq] = true;
                        acks_received++;
                        std::cout << "发送方: 收到ACK [seq=" << ack_seq << "]" << std::endl;
                        while (sender_window.base < total_packets && sender_window.ack_received[sender_window.base])
                        {
                            sender_window.base++;
                        }
                    }
                }
            }
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        std::cout << "发送方完成，base=" << sender_window.base << std::endl;
    }

    void receiver(double packet_loss_rate, double ack_loss_rate)
    {
        std::cout << "接收方启动，窗口大小: " << receiver_window.window_size << std::endl;

        while (running)
        {
            std::unique_lock<std::mutex> lock(mtx);
            if (cv.wait_for(lock, std::chrono::milliseconds(100),[this]() 
                { return !data_queue.empty() || !running; }))
            {
                if (!running && data_queue.empty()) 
                    break;

                while (!data_queue.empty())
                {
                    int packet_seq = data_queue.front();
                    data_queue.pop();
                    if (packet_seq >= receiver_window.base && packet_seq < receiver_window.base + receiver_window.window_size)
                    {

                        if (dis(gen) >= packet_loss_rate)
                        {
                            packets_received++;
                            receiver_window.sent[packet_seq] = true;
                            std::cout << "接收方: 接收数据包 [seq=" << packet_seq << "]" << std::endl;
                            if (dis(gen) >= ack_loss_rate)
                            {
                                ack_queue.push(packet_seq);
                                acks_sent++;
                                std::cout << "接收方: 发送ACK [seq=" << packet_seq << "]" << std::endl;
                            }
                            else
                            {
                                acks_lost++;
                                std::cout << "接收方: ACK丢失 [seq=" << packet_seq << "]" << std::endl;
                            }
                            while (receiver_window.base < total_packets && receiver_window.sent[receiver_window.base])
                            {
                                receiver_window.base++;
                            }
                        }
                        else
                        {
                            packets_lost++;
                            std::cout << "接收方: 数据包处理失败 [seq=" << packet_seq << "]" << std::endl;
                        }
                    }
                    else 
                    {
                        std::cout << "接收方: 数据包不在窗口内 [seq=" << packet_seq << ", 窗口=" << receiver_window.base << "-" << receiver_window.base + receiver_window.window_size - 1 << "]" << std::endl;
                        if (packet_seq < receiver_window.base)
                        {
                            if (dis(gen) >= ack_loss_rate)
                            {
                                ack_queue.push(packet_seq);
                                acks_sent++;
                                std::cout << "接收方: 发送重复ACK [seq=" << packet_seq << "]" << std::endl;
                            }
                        }
                    }
                }
            }
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        std::cout << "接收方完成，base=" << receiver_window.base << std::endl;
    }

    void display_sender_window_state()
    {
        int base = sender_window.base;
        int window_size = sender_window.window_size;
        int next_seq = sender_window.next_seq;

        std::cout << "基序号(base): " << base << std::endl;
        std::cout << "窗口范围: " << base << " - " << base + window_size - 1 << std::endl;
        std::cout << "已发送并收到确认: ";
        for (int i = 0; i < base; i++)
        {
            if (i < total_packets) std::cout << i << " ";
        }
        std::cout << std::endl;
        std::cout << "已发送但未收到确认: ";
        for (int i = base; i < next_seq; i++)
        {
            if (i < total_packets && !sender_window.ack_received[i])
                std::cout << i << " ";
        }
        std::cout << std::endl;
        std::cout << "允许发送但尚未发送: ";
        for (int i = next_seq; i < base + window_size; i++)
        {
            if (i < total_packets && !sender_window.sent[i])
                std::cout << i << " ";
        }
        std::cout << std::endl;
        std::cout << "不允许发送: ";
        for (int i = base + window_size; i < total_packets; i++)
        {
            std::cout << i << " ";
        }
        std::cout << std::endl;

        int advertised_window = window_size;
        int available_window = base + window_size - next_seq;
        std::cout << "通知窗口大小: " << advertised_window << std::endl;
        std::cout << "可用窗口大小: " << std::max(0, available_window) << std::endl;
    }

    void display_receiver_window_state()
    {
        int base = receiver_window.base;
        int window_size = receiver_window.window_size;

        std::cout << "基序号(base): " << base << std::endl;
        std::cout << "窗口范围: " << base << " - " << base + window_size - 1 << std::endl;
        std::cout << "已发送确认并交付主机: ";
        for (int i = 0; i < base; i++)
        {
            if (i < total_packets) std::cout << i << " ";
        }
        std::cout << std::endl;
        std::cout << "允许接收: ";
        for (int i = base; i < base + window_size; i++)
        {
            if (i < total_packets && !receiver_window.sent[i])
                std::cout << i << " ";
        }
        std::cout << std::endl;
        std::cout << "已接收但未交付: ";
        for (int i = base; i < base + window_size; i++)
        {
            if (i < total_packets && receiver_window.sent[i])
                std::cout << i << " ";
        }
        std::cout << std::endl;
        std::cout << "不允许接收: ";
        for (int i = base + window_size; i < total_packets; i++)
        {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }

    void display_window_state()
    {
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (!running)
                break;

            std::unique_lock<std::mutex> lock(mtx);
            std::cout << "【发送方窗口】" << std::endl;
            display_sender_window_state();
            std::cout << "【接收方窗口】" << std::endl;
            display_receiver_window_state();
            lock.unlock();
        }
    }
    void stop()
    {
        running = false;
        cv.notify_all();
    }
};

int main()
{
    const int WINDOW_SIZE = 4;
    const int TOTAL_PACKETS = 15;
    const double PACKET_LOSS_RATE = 0.2;
    const double ACK_LOSS_RATE = 0.2;

    SlidingWindowProtocol protocol(WINDOW_SIZE);
    std::cout << "窗口大小: " << WINDOW_SIZE << std::endl;
    std::cout << "总数据包数: " << TOTAL_PACKETS << std::endl;
    std::cout << "数据包丢失率: " << PACKET_LOSS_RATE * 100 << "%" << std::endl;
    std::cout << "ACK丢失率: " << ACK_LOSS_RATE * 100 << "%" << std::endl;

    std::thread sender_thread([&]() {
        protocol.sender(TOTAL_PACKETS, PACKET_LOSS_RATE, ACK_LOSS_RATE);
        });
    std::thread receiver_thread([&]() {
        protocol.receiver(PACKET_LOSS_RATE, ACK_LOSS_RATE);
        });
    std::thread display_thread([&]() {
        protocol.display_window_state();
        });

    sender_thread.join();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    protocol.stop();
    receiver_thread.join();
    display_thread.join();
    return 0;
}