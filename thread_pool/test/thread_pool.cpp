#include <catch.hpp>

#include <functional>
#include <iostream>

#include <thread_pool.hpp>

using std::cout;

SCENARIO("Parallel jobs execution")
{
	GIVEN("A thread_pool instance")
	{
		thread_pool& pool = thread_pool::get_instance(4);
		int n = 0;
		std::function<void()> job = [&n](){n++;};
		
		WHEN("A set of jobs is committed")
		{
			for (unsigned char i = 0; i < 8; i++)
			{
				pool.commit(job);
			}
			
			THEN("They are correctly executed")
			{
				//Waiting 
				while (!pool.get_status())
				{}
				
				REQUIRE(n == 8);
			}
		}
	}
}
