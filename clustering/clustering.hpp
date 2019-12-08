#ifndef clustering_hpp
#define clustering_hpp

#include <vector>
#include <type_traits>
#include <utility>
#include <functional>

template <with_computable_distance T> requires 
class clustering
{
    static_assert(std::is_copy_constructable(T)::value, "Cannot cluster with non-copy constructable types.");
    
    private:
        //Useful for assigning a cluster to an element
        struct element 
        {
            T data;
            unsigned char cluster_id;
            
            element(e, id):
                data(e), cluster_id(id)
            {}
        };
    
        //Corpus
        std::vector <element> elements;
        std::vector <T> centroids;
        
        std::function <float(T&, T&)> distance;
        std::function <bool(std::vector<T>&)> termination_condition;
    
    public:
        //Constructor with empty corpus
        clustering(std::function <float(T&, T&)>& dist, std::function <bool(std::vector<T>&)>& termination_cond):
            distance(dist), 
            termination_condition(termination_cond)
        {}
        //Constructor with defined corpus
        clustering(std::vector<T>& corpus, std::vector<T>& starting_centroids, std::function <float(T&, T&)>& dist, std::function <bool(std::vector<T>&)>& termination_cond):
            centroids(starting_centroids.begin(), starting_centroids.end()), 
            distance(dist), 
            termination_condition(termination_cond)
        {
            //Allocating memory
            elements.resize(corpus.size());
            //Copying elements from corpus
            add_element(corpus);
        }
        
        ~clustering()
        {}
        
        inline void add_element(T& e)
        {
            //0 is the default cluster for every element
            elements.emplace_back(e, 0);
        }
        void add_element(std::vector<T>& e)
        {
            for (auto& i: e)
            {
                add_element(i);
            }
        }
        
        void find_clusters()
        {
            
        }
        
        void get_clusters(std::vector<T>& clusters)
        {
            
        }
};

#endif
