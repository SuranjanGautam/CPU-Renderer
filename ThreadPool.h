#pragma once
#include<thread>
#include<mutex>
#include<vector>
#include<queue>
#include <condition_variable>

using namespace std;
class ThreadPool
{
public:
	
	bool allsent = false;	
	int working = 0;
	
	

	explicit ThreadPool(){		
		for (int i = 0;i < thread::hardware_concurrency()+1;i++) {			
			Pool.push_back(  thread(bind(&ThreadPool::run,this)));
		}
		cout << Pool.size();
	}
	~ThreadPool() {
		unique_lock<mutex> eventlock{ eventMutex };
		stop = true;		
		event.notify_all();	

		for (auto &h : Pool) {
			if (h.joinable()) { h.detach(); }
		}
	}

	void enqueue(function<void()> f) {
		unique_lock<mutex> eventlock{ eventMutex };
		TaskQueue.push(f);
		
		event.notify_one();
		eventlock.unlock();
	}

	bool allIdle() {
		bool idle (working == Pool.size());	
		return (idle);
	}

	void waitfinish() {
		unique_lock<mutex> eventlock{ eventMutex };
		wait.wait(eventlock, [this] {
			return (TaskQueue.empty() || working == 0);
		});
	}
	
	void run() {
		while (true) {
			function<void()> job;
			{
				unique_lock<mutex> eventlock{ eventMutex };				
				event.wait(eventlock);
				if (stop) {
					break;
				}
				if (!TaskQueue.empty()) {
					working += 1;
					job = TaskQueue.front();
					TaskQueue.pop();
				}
			}
			job();
			{
				unique_lock<mutex> eventlock{ eventMutex };
				working -= 1;
				wait.notify_one();
			}
			
		}
	}

private:
	condition_variable event;
	condition_variable wait;
	mutex eventMutex;	
	bool stop=false;
	vector<thread> Pool;
	queue<function<void()>> TaskQueue;	
	
};

