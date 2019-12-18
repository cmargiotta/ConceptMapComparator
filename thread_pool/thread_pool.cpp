#include "thread_pool.hpp"

#include <iostream>

using std::cout;

void thread_pool::thread_code()
{
	while(!kill_children)
	{
		//Locking mutex for safely accessing the jobs vector
		pool_mutex.lock();
		
		if (!jobs.empty())
		{
			std::function <void()> job = jobs.front();
			jobs.pop();
			
			job();
			
			//Unlocking the mutex
			pool_mutex.unlock();
		}
		else
		{
			pool_mutex.unlock();
		}
	}
}

thread_pool::thread_pool(size_t thread_number)
{	
	threads.resize(thread_number);
	
	for (size_t i = 0; i < thread_number; i++)
	{
		threads.emplace_back(&thread_pool::thread_code, this);
	}
}

thread_pool::~thread_pool()
{
	cout << "Destroying\n";
	//Killing all threads
	kill_children = true;
	
	for (auto& thread: threads)
	{
		if (thread.joinable())
			thread.join();
	}
	cout << "Done\n";
}

thread_pool& thread_pool::get_instance(size_t thread_number)
{
	static thread_pool instance (thread_number);
	
	return instance;
}

void thread_pool::commit(std::function<void()> job)
{
	std::unique_lock lock(pool_mutex);
	jobs.emplace(job);
}

bool thread_pool::get_status()
{
	return jobs.empty();
}

