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

#include <iostream>
using std::cout;
using std::endl;

template <typename T>
struct cluster
{
    std::vector<T> 		elements;
    std::vector<float> 	distances;
    T medoid;
    
    cluster(const std::vector<T>& el, const T& med, const std::vector<float>& dist):
        elements(el.begin(), el.end()),
        distances(dist.begin(), dist.end()),
        medoid(med)
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
		std::vector<std::vector<float>>         similarity_matrix;
        //medoids_assignation[i] is the index of the medoid of elements[i]'s cluster 
        std::vector<size_t>                     medoids_assignation;
        //average_distances[i] is the average distance between the medoid elements[medoids[i]] and every element in his cluster
        float                					cost;
		
		std::function <float(const T&, const T&)>			distance_lambda;
        
        //Random number generator
        std::default_random_engine              generator;
        std::uniform_int_distribution<size_t>   distribution;
        
        float distance(size_t el1, size_t el2)
        {
			size_t i = std::min(el1, el2);
			size_t j = std::max(el1, el2);

			//If it is not stored in the matrix, compute it
			if (similarity_matrix[i][j] == -1.0f)
			{
				similarity_matrix[i][j] = distance_lambda(elements[i], elements[j]);
				return similarity_matrix[i][j];
			}

			return similarity_matrix[i][j];
		}
        
        void assign_medoids()
        {
			thread_pool& pool = thread_pool::get_instance();

            for (size_t i = 0; i < elements.size(); i++)
            {
                std::function <void()> job = [this, i]()
                {
                    float min = std::numeric_limits<float>::max(), tmp;
                    size_t min_medoid = this->medoids[0];
                    
                    for (size_t m: this->medoids)
                    {
						tmp = this->distance(i, m);
						
                        if (tmp < min)
                        {
                            min = tmp;
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
            
            cost = compute_cost();
        }
        
        float compute_cost()
        {
			float cost = 0.0f;
			
            for (size_t i = 0; i < elements.size(); i++)
            {
				cost += distance(i, medoids_assignation[i]);
            }
            
            return cost;
        }
        
        bool check_new_medoid(size_t new_medoid)
        {
			std::vector<size_t> backup (medoids_assignation.begin(), medoids_assignation.end());
			size_t to_replace = medoids_assignation[new_medoid];
			
			for (size_t& m: medoids)
			{
				if (m == to_replace)
				{
					m = new_medoid;
                    break;
				}
			}
			
            float old_cost = cost;
            assign_medoids();
                        
            if (old_cost > cost)
            {
				return true;
			}
			
			for (size_t& m: medoids)
			{
				if (m == new_medoid)
				{
					m = to_replace;
					break;
				}
			}
			//Resetting backup
			for (size_t i = 0; i < backup.size(); i++)
			{
				medoids_assignation[i] = backup[i];
			}
            cost = old_cost;
			
			return false;
        }
        
        bool select_new_random_medoid()
        {
            size_t random_index = distribution(generator);

			if (check_new_medoid(random_index))
			{	
				return true;
			}
            
            return false;
        }
        
        bool assign_clusters()
        {
            std::vector<T> clust;
            std::vector<float> dist;
            //Every cluster, on average, has elements/clusters objects
            clust.reserve(elements.size()/medoids.size());
            dist.reserve(elements.size()/medoids.size());
            
            for (size_t medoid_index: medoids)
            {
				dist.clear();
                clust.clear();
                for (size_t i = 0; i < elements.size(); i++)
                {
                    if (medoids_assignation[i] == medoid_index)
                    {
						dist.push_back(distance(medoid_index, i));
                        clust.push_back(elements[i]);
                    }
                }
                
                valid_clusters.emplace_back(clust, elements[medoid_index], dist);
            }
            
            return valid_clusters.size() != 0;
        }
	
	public:
		//Constructor
		clustering( const std::vector<T>& corpus, 
                    const std::vector<size_t>& starting_medoids, 
                    std::function <float(const T&, const T&)>& dist 
                  ):
            elements(corpus.begin(), corpus.end()),
			medoids(starting_medoids.begin(), starting_medoids.end()), 
			similarity_matrix(elements.size(), std::vector<float>(elements.size(), -1.0f)),
            medoids_assignation(elements.size(), 0),
			distance_lambda(dist), 
            distribution(0, elements.size()-1)
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
		}
		
		//Constructor with random medoids
		clustering( const std::vector<T>& corpus, 
                    size_t medoids_number, 
                    std::function <float(const T&, const T&)>& dist 
                  ):
            elements(corpus.begin(), corpus.end()),
			similarity_matrix(elements.size(), std::vector<float>(elements.size(), -1.0f)),
            medoids_assignation(elements.size(), 0),
			distance_lambda(dist), 
            distribution(0, elements.size()-1)
		{
            if (elements.size() == 0)
            {
                throw std::runtime_error("Empty corpus.");
            }
            
            medoids.reserve(medoids_number);
            for (size_t i = 0; i < medoids_number; i++)
            {
				medoids.push_back(distribution(generator));
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
		}
		
		//Destructor
		~clustering()
		{}
		
		void find_clusters()
		{
			assign_medoids();
			
            //Waiting for convergence
            for (size_t i = 0; i < elements.size(); i++)
            {
                if (check_new_medoid(i))
                {
					i = 0;
				}
            }
            
            assign_clusters();
		}
		
		const std::vector<cluster<T>>& get_clusters() const noexcept
		{
			return valid_clusters;
		}
};

#endif
