#include "similarity.hpp"

#include <vector>
#include <list>
#include <utility>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>

#include "wordnet.hpp"

using namespace std;

synset similarity::LCS(const synset& word1, const synset& word2)
{
	std::set<string> intersection;

	for (const auto& hyperynm1: word1.hypernym_path)
	{
		for (const auto& hypernym2: word2.hypernym_path)
		{
			if (hyperynm1.first == hypernym2.first)
			{
				intersection.insert(hyperynm1.first);
			}
		}
	}
		
	string best_common_parent = *min_element(intersection.begin(), intersection.end(), [&word1, &word2](auto el1, auto el2)
	{
		return (word1.hypernym_path.at(el1)+word2.hypernym_path.at(el1)) < (word1.hypernym_path.at(el2)+word2.hypernym_path.at(el2));
	});
	
	return synset(best_common_parent);
}

string similarity::nearest_common_semfield(const synset& word1, const synset& word2)
{
	std::set<string> intersection;
	
	for (const auto& sem1: word1.semfield_path)
	{
		for (const auto& sem2: word2.semfield_path)
		{
			if (sem1.first == sem2.first)
			{
				intersection.insert(sem1.first);
			}
		}
	}
	
	if (intersection.size() == 0)
	{
		return "";
	}
		
	string best_common_semfield = *min_element(intersection.begin(), intersection.end(), [&word1, &word2](auto el1, auto el2)
	{
		return (word1.semfield_path.at(el1)+word2.semfield_path.at(el1)) < (word1.semfield_path.at(el2)+word2.semfield_path.at(el2));
	});
	
	return best_common_semfield;
}

float similarity::informative_content(const synset& word)
{
	static map<string, float> cache;
	
	try 
	{
		return cache.at(word.id);
	}
	catch (...)
	{
		wordnet& wn = wordnet::get_instance();
		
		float ic = abs(1.0f - log(word.hyponym_count+1.0f)/log(wn.get_concept_number()));
		cache[word.id] = ic;
		
		return ic;
	}
}

float similarity::compare_words(const synset& word1, const synset& word2)
{	
	static map<pair<string, string>, float> cache;
	
	try 
	{
		return cache.at(make_pair(word1.id, word2.id));
	}
	catch (...)
	{
		synset lcs 		= LCS(word1, word2);

		string semfield = nearest_common_semfield(word1, word2);
			
		float sim = 2.0f*informative_content(lcs)/(informative_content(word1) + informative_content(word2));
		sim *= 0.5f;

		if (semfield != "")
		{
			sim += 1.0f/(word1.semfield_path.at(semfield) + word2.semfield_path.at(semfield));
		}
		
		cache[make_pair(word1.id, word2.id)] = sim;
		cache[make_pair(word2.id, word1.id)] = sim;
			
		return sim;
	}
}
