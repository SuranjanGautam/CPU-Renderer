#pragma once
#include <condition_variable>
#include <functional>
#include <iostream>
#include <future>
#include <vector>
#include <thread>
#include <queue>

class ThreadPool
{
public:
	using Task = std::function<void()>;

	explicit ThreadPool()
	{
		start();
	}

	~ThreadPool()
	{
		stop();
	}

	template<class T>
	auto enqueue(T task)->std::future<decltype(task())>
	{
		auto wrapper = std::make_shared<std::packaged_task<decltype(task()) ()>>(std::move(task));

		{
			std::unique_lock<std::mutex> lock{ mEventMutex };
			mTasks.emplace([=] {
				(*wrapper)();
			});
		}

		mEventVar.notify_one();
		return wrapper->get_future();
	}

	void waitfinsh() {
		unique_lock<mutex> eventlock{ mEventMutex };
		wait.wait(eventlock, [&] {
			return (mTasks.empty() && working == 0);
		});
	}

private:
	int working = 0;

	std::vector<std::thread> mThreads;

	std::condition_variable mEventVar;

	condition_variable wait;

	std::mutex mEventMutex;
	bool mStopping = false;

	std::queue<Task> mTasks;

	void start()
	{
		int threadcount;
		threadcount = std::fmax(2, thread::hardware_concurrency());
		for (auto i = 0u; i < thread::hardware_concurrency(); ++i)
		{
			mThreads.emplace_back([=] {
				while (true)
				{
					Task task;

					{
						std::unique_lock<std::mutex> lock{ mEventMutex };
						mEventVar.wait(lock, [=] { return mStopping || !mTasks.empty(); });

						if (mStopping && mTasks.empty())
							break;
						working++;
						task = std::move(mTasks.front());
						mTasks.pop();
					}
					task();
					{
						std::unique_lock<std::mutex> lock{ mEventMutex };
						working--;
					}						
						wait.notify_one();
					
				}
			});
		}
	}

	void stop() noexcept
	{
		{
			std::unique_lock<std::mutex> lock{ mEventMutex };
			mStopping = true;
		}

		mEventVar.notify_all();

		for (auto &thread : mThreads)
			thread.join();
	}
};
