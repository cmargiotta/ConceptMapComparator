#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <functional>

#include <clustering.hpp>
#include <thread_pool.hpp>

using namespace std;

int main()
{
	time_t start, end; 
	time(&start); 
	thread_pool::get_instance(16);
	
	vector <short> 						corpus (100000, 0);
	default_random_engine              	generator;
    uniform_int_distribution<short>   	distribution;
	
	for (size_t i = 0; i < corpus.size(); i++)
	{
		corpus[i] = i;
	}
	
	function<float(short&, short&)> distance = [](short n1, short n2){return (float) abs((float) n1-n2);};
	function<bool(std::vector<short>&, size_t, float)> termination_condition = [&corpus](std::vector<short>&, size_t, float average_distance)
	{
		return average_distance < corpus.size()/8.0f;
	};
	
	clustering c (corpus, {0, 9999}, distance, termination_condition);
	c.find_clusters();
	
	auto clusters = c.get_clusters();
	time(&end);
	
	for (size_t i = 0; i < clusters.size(); i++)
	{
		cout << "Cluster " << i << "\n";
		cout << "Average distance " << clusters[i].average_distance  << '\n';
		cout << "Medoid " << clusters[i].elements[clusters[i].medoid] << " (index " << clusters[i].medoid << ")" <<'\n';
		cout << "Elements:" << clusters[i].elements.size() << "\n\n";
		
		/*
		for (auto& e: clusters[i].elements)
		{
			cout << e << '\n';
		}
		*/
		
		
		cout << '\n';
	}
	
	cout << "Time taken: " << double(end-start) << " sec\n";
}
