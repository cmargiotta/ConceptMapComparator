#ifndef clustering_hpp
#define clustering_hpp

#include <vector>
#include <type_traits>
#include <utility>
#include <functional>
#include <limits>
#include <random>
#include <stdexcept>
#include <algorithm>

template <typename T> 
class clustering
{
	static_assert(std::is_copy_constructible<T>::value, "Cannot cluster with non-copy constructable types.");
	
	private:
		//Corpus
		std::vector <T>                         elements;
		std::vector <T>                         valid_cluster;
        //elements[medoids[i]] is the i-th medoid
		std::vector <size_t>                    medoids;
        //similarity_matrix[i][j] is the distance between elements[i] and elements[j]
		float**                                 similarity_matrix;
        //medoids_assignation[i] is the index of the medoid of elements[i]'s cluster 
        size_t*                                 medoids_assignation;
        //average_distances[i] is the average distance between the medoid elements[medoids[i]] and every element in his cluster
        float*                                  average_distances;
		
		std::function <float(T&, T&)>           distance;
        //If a cluster satisfies this predicate, the algorithm ends
		std::function <bool(std::vector<T>&)>   termination_condition;
        
        //Random number generator
        std::default_random_engine              generator;
        std::uniform_int_distribution<size_t>   distribution;
        
        void compute_similarity_matrix()
        {
            for (size_t i = 0; i < elements.size(); i++)
            {
                for (size_t j = 0; j < elements.size(); j++)
                {
                    if (i <= 1 + elements.size()/2)
                        similarity_matrix[i][j] = distance(elements[i], elements[j]);
                    else 
                        similarity_matrix[i][j] = similarity_matrix[j][i];
                }
            }
        }
        
        void assign_medoids()
        {
            float min;
            size_t min_medoid;
            
            for (size_t i = 0; i < elements.size(); i++)
            {
                min = std::numeric_limits<float>::max();
                for (size_t m: medoids)
                {
                    if (similarity_matrix[i][m] < min)
                    {
                        min = similarity_matrix[i][m];
                        min_medoid = m;
                    }
                }
                
                medoids_assignation[i] = min_medoid;
            }
            
            compute_average_distances();
        }
        
        void compute_average_distances()
        {
            std::vector<size_t> counters (medoids.size(), 0);
            //Cleaning array
            for (size_t i = 0; i < medoids.size(); i++)
            {
                average_distances[i] = 0.0f;
            }
            
            for (size_t i = 0; i < elements.size(); i++)
            {
                //Increasing counter
                counters[medoids_assignation[i]]++;
                //Adding new distance to average (av = av + (x-av)/(n+1))
                average_distances[medoids_assignation[i]] += ((similarity_matrix[i][medoids_assignation[i]] - average_distances[medoids_assignation[i]])/counters[medoids_assignation[i]]);
            }
        }
        
        float cost(size_t new_medoid)
        {
            size_t old_medoid = medoids_assignation[new_medoid], n = 0;
            float old_average = average_distances[old_medoid];
            float new_average = 0.0f;
            
            //Computing average distance for the new medoid
            for (size_t i = 0; i < elements.size(); i++)
            {
                if (medoids_assignation[i] == old_medoid)
                {
                    n++;
                    new_average += ((similarity_matrix[i][new_medoid] - new_average)/n);
                }
            }
            
            return old_average - new_average;
        }
        
        bool select_new_random_medoid()
        {
            size_t random_index = distribution(generator);
            float switching_cost = cost(random_index);
            
            if (switching_cost < 0)
            {
                size_t old_medoid = medoids_assignation[random_index];
                for (size_t& m: medoids)
                {
                    if (m == old_medoid)
                    {
                        m = random_index;
                    }
                }
                assign_medoids();
                
                return true;
            }
            
            return false;
        }
        
        bool check_termination_condition()
        {
            std::vector<T> cluster;
            //Every cluster, on average, has elements/clusters objects
            cluster.resize(elements.size()/medoids.size());
            
            for (size_t medoid_index: medoids)
            {
                cluster.clear();
                for (size_t i = 0; i < elements.size(); i++)
                {
                    if (medoids_assignation[i] == medoid_index)
                    {
                        cluster.push_back(elements[i]);
                    }
                }
                
                if (termination_condition(cluster))
                {
                    valid_cluster.insert(valid_cluster.begin(), cluster.begin(), cluster.end());
                    return true;
                }
            }
            
            return false;
        }
	
	public:
		//Constructor
		clustering( const std::vector<T>& corpus, 
                    const std::vector<size_t>& starting_medoids, 
                    std::function <float(T&, T&)>& dist, 
                    std::function <bool(std::vector<T>&)>& termination_cond
                  ):
            elements(corpus.begin(), corpus.end()),
			medoids(starting_medoids.begin(), starting_medoids.end()), 
			distance(dist), 
			termination_condition(termination_cond),
            distribution(0, elements.size())
		{
            if (medoids.size() == 0)
            {
                throw std::runtime_error("Cannot run clustering without medoids.");
            }
            if (elements.size() == 0)
            {
                throw std::runtime_error("Empty corpus.");
            }
            if (!std::all_of(medoids.begin(), 
                             medoids.end(), 
                             [this](size_t el){
                                    return el < this->elements.size();
                             })
                )
            {
                throw std::runtime_error("Every medoid has to be part of the corpus.");
            }
            
            //Allocating matrix memory
            similarity_matrix   = new float*[elements.size()];
            medoids_assignation = new size_t[elements.size()];
            average_distances    = new float[medoids.size()];
            
            for (size_t i = 0; i < elements.size(); i++)
            {
                similarity_matrix[i]  = new float[elements.size()];
            }
            
            compute_similarity_matrix();
		}
		
		~clustering()
		{
            for (size_t i = 0; i < elements.size(); i++)
            {
                delete similarity_matrix[i];
            }
            
            delete medoids_assignation;
            delete similarity_matrix;
            delete average_distances; 
        }
		
		void find_clusters()
		{
			assign_medoids();
            bool termination_check = check_termination_condition();
            
            while (!termination_check)
            {
                //Selecting new random medoid
                while (!select_new_random_medoid())
                {}
                
                termination_check = check_termination_condition();
            }
		}
		
		void get_clusters(std::vector<T>& clusters) const
		{
			clusters.insert(clusters.begin(), valid_cluster.begin(), valid_cluster.end());
		}
};

#endif
