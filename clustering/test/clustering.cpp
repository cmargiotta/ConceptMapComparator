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
		function<bool(vector<int>&, size_t, float)> term = [](vector<int>&, size_t, float){return true;};
		
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
	GIVEN("A set of invalid medoids")
	{
		vector<int> corpus ({1, 2, 3, 4});
		vector<size_t> medoids({99, 100});
		
		function<float(int&, int&)> dist  = [](int&, int&){return 0.0f;};
		function<bool(vector<int>&, size_t, float)> term = [](vector<int>&, size_t, float){return true;};
		
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

SCENARIO("Execution")
{
	GIVEN("A valid dataset and a valid set of starting medoids")
	{
		vector<int> corpus ({1, 2, 3, 4, 95, 96, 97, 98});
		vector<size_t> medoids({2, 6});
		
		function<float(int&, int&)> dist  = [](int& x, int& y){return abs((float)x-(float)y);};
		function<bool(vector<int>&, size_t, float)> term = [](vector<int>&, size_t, float){return true;};
		
		clustering<int> *c = nullptr;
		
		REQUIRE_NOTHROW(c = new clustering<int>(corpus, medoids, dist, term));
		REQUIRE(c != nullptr);
		vector<int> expected1 ({1, 2, 3, 4}), expected2 ({95, 96, 97, 98});
		
		WHEN("Its clustering is computed")
		{
			REQUIRE_NOTHROW(c->find_clusters());
			
			THEN("They are correctly computed")
			{
				auto clusters = c->get_clusters();
					
				REQUIRE_THAT(clusters[0].elements, UnorderedEquals(expected1));
				REQUIRE_THAT(clusters[1].elements, UnorderedEquals(expected2));
				REQUIRE(clusters.size() == 2);
			}
		}
		
		if (c)
		{
			delete c;
		}
	}
	GIVEN("A valid dataset and an invalid set of starting medoids")
	{
		vector<int> corpus ({1, 2, 3, 4, 95, 96, 97, 98});
		vector<size_t> medoids({2, 3});
		
		function<float(int&, int&)> dist  = [](int& x, int& y){return abs((float)x-(float)y);};
		function<bool(vector<int>&, size_t, float)> term = [](vector<int>& c, size_t, float average_dist){
			return average_dist <= 10.0f && c.size() == 4;
		};
		
		clustering<int> *c = nullptr;
		
		REQUIRE_NOTHROW(c = new clustering<int>(corpus, medoids, dist, term));
		REQUIRE(c != nullptr);
		vector<int> expected1 ({1, 2, 3, 4}), expected2 ({95, 96, 97, 98});
		
		WHEN("Its clustering is computed")
		{
			REQUIRE_NOTHROW(c->find_clusters());
			
			THEN("They are correctly computed")
			{
				auto clusters = c->get_clusters();
					
				REQUIRE_THAT(clusters[0].elements, UnorderedEquals(expected1));
				REQUIRE_THAT(clusters[1].elements, UnorderedEquals(expected2));
				REQUIRE(clusters.size() == 2);
			}
		}
		
		if (c)
		{
			delete c;
		}
	}
}

