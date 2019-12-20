#ifndef clustering_hpp
#define clustering_hpp

#include <vector>
#include <type_traits>
#include <utility>
#include <functional>
#include <limits>
#include <random>
#include <stdexcept>
#include <map>
#include <algorithm>

#include <thread_pool.hpp>

template <typename T>
struct cluster
{
    std::vector<T> elements;
    size_t medoid;
    float average_distance;
    
    cluster(std::vector<T>& el, size_t med, float dist):
        elements(el.begin(), el.end()),
        medoid(med),
        average_distance(dist)
    {}
};

template <typename T> 
class clustering
{
	static_assert(std::is_copy_constructible<T>::value, "Cannot cluster with non-copy constructable types.");
	
	private:
		//Corpus
		std::vector <T>                         elements;
		std::vector <cluster<T>>                valid_clusters;
        //elements[medoids[i]] is the i-th medoid
		std::vector <size_t>                    medoids;
        //similarity_matrix[i][j] is the distance between elements[i] and elements[j]
		float**                                 similarity_matrix;
        //medoids_assignation[i] is the index of the medoid of elements[i]'s cluster 
        size_t*                                 medoids_assignation;
        //average_distances[i] is the average distance between the medoid elements[medoids[i]] and every element in his cluster
        std::map<size_t, float>                 average_distances;
		
		std::function <float(T&, T&)>           distance;
        //If a cluster satisfies this predicate, the algorithm ends
		std::function <bool(std::vector<T>&, size_t, float)>   termination_condition;
        
        //Random number generator
        std::default_random_engine              generator;
        std::uniform_int_distribution<size_t>   distribution;
        
        void compute_similarity_matrix()
        {
            auto& pool = thread_pool::get_instance();
            
            for (size_t i = 0; i < elements.size(); i++)
            {
                std::function<void()> job = [this, i]()
                {
                    for (size_t j = 0; j <= i; j++)
                    {
                        this->similarity_matrix[i][j] = this->distance(elements[i], elements[j]);
                        this->similarity_matrix[j][i] = this->similarity_matrix[i][j];
                    }
                };
                
                pool.commit(job);
            }
            
            //Waiting for the jobs to finish
            while (!pool.get_status())
            {}
        }
        
        void assign_medoids()
        {
            auto& pool = thread_pool::get_instance();
                
            for (size_t i = 0; i < elements.size(); i++)
            {
                std::function <void()> job = [this, i]()
                {
                    float min = std::numeric_limits<float>::max();
                    size_t min_medoid;
                    
                    for (size_t m: this->medoids)
                    {
                        if (this->similarity_matrix[i][m] < min)
                        {
                            min = this->similarity_matrix[i][m];
                            min_medoid = m;
                        }
                    }
                    
                    this->medoids_assignation[i] = min_medoid;
                };
                
                pool.commit(job);
            }
            
            //Waiting for the jobs to finish
            while (!pool.get_status())
            {}
            
            compute_average_distances();
        }
        
        void compute_average_distances()
        {
            std::map<size_t, size_t> counters;
            
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
            
            return new_average - old_average;
        }
        
        bool select_new_random_medoid()
        {
            size_t random_index = distribution(generator);
            float switching_cost = cost(random_index);

            if (switching_cost < 0)
            {
                //old_medoid is the medoid of the selected random element
                size_t old_medoid = medoids_assignation[random_index];
                //Inserting new medoid in medoids vector
                for (size_t& m: medoids)
                {
                    if (m == old_medoid)
                    {
                        m = random_index;
                        break;
                    }
                }
                //Recomputing clusters
                assign_medoids();
                
                return true;
            }
            
            return false;
        }
        
        bool check_termination_condition()
        {
            std::vector<T> clust;
            //Every cluster, on average, has elements/clusters objects
            clust.resize(elements.size()/medoids.size());
            
            for (size_t medoid_index: medoids)
            {
                clust.clear();
                for (size_t i = 0; i < elements.size(); i++)
                {
                    if (medoids_assignation[i] == medoid_index)
                    {
                        clust.push_back(elements[i]);
                    }
                }
                
                if (termination_condition(clust, medoid_index, average_distances[medoid_index]))
                {
                    valid_clusters.emplace_back(clust, medoid_index, average_distances[medoid_index]);
                }
            }
            
            return valid_clusters.size() != 0;
        }
	
	public:
		//Constructor
		clustering( const std::vector<T>& corpus, 
                    const std::vector<size_t> starting_medoids, 
                    std::function <float(T&, T&)>& dist, 
                    std::function <bool(std::vector<T>&, size_t, float)>& termination_cond
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
        }
		
		void find_clusters()
		{
			assign_medoids();
            
            while (!check_termination_condition())
            {
                //Selecting new random medoid
                while (select_new_random_medoid())
                {}
            }
		}
		
		const std::vector<cluster<T>>& get_clusters() const noexcept
		{
			return valid_clusters;
		}
};

#endif
