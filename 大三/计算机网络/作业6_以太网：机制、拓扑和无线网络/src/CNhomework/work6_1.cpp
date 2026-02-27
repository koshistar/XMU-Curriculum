#include<iostream>
#include<vector>
#include<thread>
#include<random>
#include<queue>
#include<mutex>
#include<condition_variable>
#include<atomic>
#include<chrono>

using namespace std;

#define NUM_STATIONS 5 //站点数量
#define CHANNEL_CAPACITY 1000000 //信道容量1Mbps
#define PROPAGATION_DELAY 10 //传播延迟10μs
#define SLOT_TIME 512 //时槽时间
#define MAX_RETRIES 16 //最大重试次数
#define JAM_SIGNAL_LENGTH 32 //阻塞信号长度

atomic<bool> channel_busy{ false };
atomic<int> current_transmitter{ -1 };
mutex channel_mutex;
condition_variable channel_cv;

class Station
{
private:
	int id;
	atomic<bool> has_frame{ false };
	atomic<bool> transmitting{ false };
	atomic<bool> collision_detected{ false };
	int retry_count{ 0 };
	int backoff_time{ 0 };
	queue<vector<int> > frame_queue;
	//随机数生成器
	static mt19937 gen;
	uniform_int_distribution<> dis;
public:
	Station(int station_id) :id(station_id), dis(0, 100)
	{
		random_device rand;
		gen.seed(rand());
		for (int i = 0; i < 3; i++)
		{
			int frame_size = 512 + dis(gen) * 8;
			vector<int> frame(frame_size, 1);// 用1填充帧
			frame_queue.push(frame);
		}
	}
	// 禁用拷贝
	Station(const Station&) = delete;
	Station& operator=(const Station&) = delete;
	// 载波侦听
	bool carrier_sense()
	{
		return !channel_busy.load();
	}
	// 冲突检测
	void check_collision()
	{
		if (transmitting && current_transmitter != id)
		{
			collision_detected = true;
			cout << "Station " << id << " checks collision." << endl;
		}
	}
	// 二进制指数退避算法
	int binary_exponential_backoff()
	{
		int k = min(retry_count, 10);
		int max_backoff = (1 << k) - 1;
		uniform_int_distribution<> backoff_dis(0, max_backoff);
		return backoff_dis(gen) * SLOT_TIME;
	}

	void send_frame()
	{
		if (frame_queue.empty())
			return;
		auto frame = frame_queue.front();
		int frame_duration = frame.size() * 1000000 / CHANNEL_CAPACITY;
		{
			lock_guard<mutex> lock(channel_mutex);
			if (!carrier_sense())
				return; // 信道忙，延迟发送
			//占用信道
			channel_busy = true;
			current_transmitter = id;
			transmitting = true;
			collision_detected = false;
		}
		cout << "Station " << id << " sends frame." << endl;

		// 分时隙检测冲突
		int time_slots = frame_duration / PROPAGATION_DELAY;
		for (int i = 0; i < time_slots; i++)
		{
			this_thread::sleep_for(chrono::microseconds(PROPAGATION_DELAY));

			check_collision();
			if (collision_detected)
			{
				this_thread::sleep_for(chrono::microseconds(JAM_SIGNAL_LENGTH * 1000000 / CHANNEL_CAPACITY));
				retry_count++;
				if (retry_count > MAX_RETRIES)
				{
					cout << "Station " << id << "'s retry count is over max times. Discard this frame." << endl;
					frame_queue.pop();
					break;
				}
				backoff_time = binary_exponential_backoff();
				cout << "Station " << id << " retry " << backoff_time << "μs." << endl;
				break;
			}
		}
		// 传输完成或冲突发生
		{
			lock_guard<mutex>lock(channel_mutex);
			if (!collision_detected && transmitting)
			{
				cout << "Station " << id << " send successfully." << endl;
				frame_queue.pop();
				retry_count = 0;
			}
			channel_busy = false;
			current_transmitter = -1;
			transmitting = false;
		}
		channel_cv.notify_all();
	}


	void run()
	{
		while (!frame_queue.empty())
		{
			this_thread::sleep_for(chrono::milliseconds(100 + dis(gen) * 10));
			if (collision_detected && backoff_time > 0)
			{
				this_thread::sleep_for(chrono::microseconds(backoff_time));
				backoff_time = 0;
				collision_detected = false;
			}
			send_frame();
		}
		cout << "Station " << id << " complete sending all frames." << endl;
	}
};
mt19937 Station::gen;
//监控线程
void monitor_thread()
{
	while (true)
	{
		this_thread::sleep_for(chrono::milliseconds(500));
		cout << "Channel " << (channel_busy ? "busy" : "leisure") << endl;
		if (channel_busy)
		{
			cout << "Current station: " << current_transmitter << endl;
		}
	}
}
int main()
{
	vector<Station*> stations;
	vector<thread> station_threads;
	for (int i = 0; i < NUM_STATIONS; i++)
	{
		stations.emplace_back(new Station(i));
	}
	thread monitor(monitor_thread);
	for (int i = 0; i < NUM_STATIONS; i++)
	{
		station_threads.emplace_back([&stations, i]() {
			stations[i]->run();
		});
	}
	for (auto& t : station_threads)
	{
		t.join();
	}
	monitor.detach();
	return 0;
}