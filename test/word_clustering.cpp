#include <catch.hpp>

#include <vector>
#include <functional>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <string>

#include <clustering.hpp>
#include <thread_pool.hpp>
#include <wordnet.hpp>
#include <similarity.hpp>

using namespace std;
using namespace Catch::Matchers;

SCENARIO("Clustering a set of words")
{
	GIVEN("A corpus of words, a distance function and a termination condition")
	{
		//Using 4 threads
		thread_pool::get_instance(4);
		wordnet& wn = wordnet::get_instance("../wordnet/dict");
		
		vector<synset> corpus;
		//Adding "entity" is useful for clustering unrelated synsets
		corpus.emplace_back(wn.get_entity_id());
		for_each(wn.get_id("hamster").begin(), wn.get_id("hamster").end(), [&corpus](auto& el){corpus.emplace_back(el);});
		for_each(wn.get_id("dog").begin(), wn.get_id("dog").end(), [&corpus](auto& el){corpus.emplace_back(el);});
		for_each(wn.get_id("mouse").begin(), wn.get_id("mouse").end(), [&corpus](auto& el){corpus.emplace_back(el);});
		
		function <float(const synset&, const synset&)> dist ([](const synset& s1, const synset& s2)
		{
			wordnet& wn = wordnet::get_instance();
			float sim = similarity::compare_words(s1, s2);
			
			//Similarity threshold
			if ((s1.id != wn.get_entity_id() && s2.id != wn.get_entity_id()) && sim <= 0.3f)
				sim = 0.0f;
			
			return 1.0f - sim;
		});
		
        function <bool(std::vector<synset>&, const synset&)> termination_condition ([](std::vector<synset>& clust, const synset&)
        {
			wordnet& wn = wordnet::get_instance();
			bool found1 = false, found2 = false, found3 = false;
			
			for (auto& s: clust)
			{
				if (wn.get_word(s.id) == "hamster")
					found1 = true;
				if (wn.get_word(s.id) == "dog")
					found2 = true;
				if (wn.get_word(s.id) == "mouse")
					found3 = true;
			}
			
			return found1 && found2 && found3;
		});
		
		WHEN("A clustering of them is computed")
		{
			clustering c (corpus, {0, 1, 2}, dist, termination_condition);
			c.find_clusters();
			
			const vector<cluster<synset>>& clusters = c.get_clusters();
			
			for(auto c: clusters[0].elements) cout << wn.get_word(c.id) << "  " << c.id << endl;
			
			REQUIRE(find(clusters[0].elements.begin(), clusters[0].elements.end(), synset(wn.get_id("hamster")[0])) != clusters[0].elements.end());
			REQUIRE(find(clusters[0].elements.begin(), clusters[0].elements.end(), synset(wn.get_id("dog")[0])) != clusters[0].elements.end());
			REQUIRE(find(clusters[0].elements.begin(), clusters[0].elements.end(), synset(wn.get_id("mouse")[0])) != clusters[0].elements.end());
		}
	}
}
