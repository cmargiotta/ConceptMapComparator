#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <thread>
#include <vector>
#include <mutex> 
#include <functional>
#include <queue>

// SINGLETON
class thread_pool
{
	private:
		std::mutex 									pool_mutex;
		std::vector<std::thread> 					threads;
		std::queue<std::function<void()>>	jobs;
		bool										kill_children {false};
		
		void thread_code();
		//Private constructor
		thread_pool(size_t thread_number);
		~thread_pool();
	public:
		//WARNING: Only the thread_number in the first call will be considered
		static thread_pool& get_instance(size_t thread_number = 0);
		
		//Inlining this function allows to call commit(...) without performance issues when threads.size() is 0
		inline void commit(std::function<void()> job)
		{
			if (threads.size() != 0)
			{
				std::unique_lock lock(pool_mutex);
				jobs.emplace(job);
			}
			else
			{
				job();
			}
		}
		//Returns true if the jobs queue is empty
		bool get_status();
};

#endif
