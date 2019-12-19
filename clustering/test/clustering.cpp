#include <catch.hpp>

#include <vector>
#include <functional>
#include <cmath>
#include <algorithm>

#include <clustering.hpp>
#include <thread_pool.hpp>

using namespace std;
using namespace Catch::Matchers;

SCENARIO("Initialization")
{
	//Using 4 threads
	thread_pool::get_instance(4);
	
	GIVEN("An empty corpus")
	{
		vector<int> corpus;
		vector<size_t> medoids;
		
		function<float(int&, int&)> dist  = [](int&, int&){return 0.0f;};
		function<bool(vector<int>&)> term = [](vector<int>&){return true;};
		
		WHEN("A clustering instance is initialized")
		{
			clustering <int>* c = nullptr;
			
			THEN("An error is thrown")
			{
				REQUIRE_THROWS(c = new clustering<int>(corpus, medoids, dist, term));
				
				if (c != nullptr)
				{
					delete c;
				}
			}
		}
	}
	GIVEN("A set of non-valid medoids")
	{
		vector<int> corpus ({1, 2, 3, 4});
		vector<size_t> medoids({99, 100});
		
		function<float(int&, int&)> dist  = [](int&, int&){return 0.0f;};
		function<bool(vector<int>&)> term = [](vector<int>&){return true;};
		
		WHEN("A clustering instance is initialized")
		{
			clustering <int>* c = nullptr;
			
			THEN("An error is thrown")
			{
				REQUIRE_THROWS(c = new clustering<int>(corpus, medoids, dist, term));
				
				if (c != nullptr)
				{
					delete c;
				}
			}
		}
	}
}

SCENARIO("Clustering a valid dataset")
{
	GIVEN("A valid dataset and the relative clustering instance")
	{
		vector<int> corpus ({1, 2, 3, 4, 95, 96, 97, 98});
		vector<size_t> medoids({2, 6});
		
		function<float(int&, int&)> dist  = [](int& x, int& y){return abs((float)x-(float)y);};
		function<bool(vector<int>&)> term = [](vector<int>&){return true;};
		
		clustering<int> *c = nullptr;
		
		REQUIRE_NOTHROW(c = new clustering<int>(corpus, medoids, dist, term, true));
		REQUIRE(c != nullptr);
		vector<int> cluster, expected ({1, 2, 3, 4});
		
		WHEN("Its clustering is computed")
		{
			REQUIRE_NOTHROW(c->find_clusters());
			
			THEN("They are correctly computed")
			{
				c->get_clusters(cluster);
					
				REQUIRE_THAT(cluster, UnorderedEquals(expected));
			}
		}
		
		if (c)
		{
			delete c;
		}
	}
}

