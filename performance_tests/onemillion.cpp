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
	
	function<float(const short&, const short&)> distance = [](const short& n1, const short& n2){return (float) abs((float) n1-n2);};
	function<bool(std::vector<short>&, const short&)> termination_condition = [&corpus](std::vector<short>&, const short&)
	{
		return true;
	};
	
	clustering c (corpus, {0, 9999}, distance, termination_condition);
	c.find_clusters();
	
	auto clusters = c.get_clusters();
	time(&end);
	
	for (size_t i = 0; i < clusters.size(); i++)
	{
		cout << "Cluster " << i << "\n";
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
