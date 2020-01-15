#include "similarity.hpp"

#include <vector>
#include <list>
#include <utility>
#include <algorithm>
#include <cmath>
#include <iostream>

#include "wordnet.hpp"

using namespace std;

unsigned int similarity::LCS(const synset& word1, const synset& word2)
{
	std::set<unsigned int> intersection;
	
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
		
	unsigned int best_common_parent = *min_element(intersection.begin(), intersection.end(), [&word1, &word2](auto el1, auto el2)
	{
		return (word1.hypernym_path.at(el1)+word2.hypernym_path.at(el1)) < (word1.hypernym_path.at(el2)+word2.hypernym_path.at(el2));
	});
	
	return best_common_parent;
}

float similarity::informative_content(unsigned int word)
{
	wordnet& wn = wordnet::get_instance();
	
	return 1 - log(wn.get_hyponym_count(word)+1)/log(wn.get_concept_number());
}

float similarity::compare_words(const synset& word1, const synset& word2)
{	
	unsigned int lcs = LCS(word1, word2);
	
	return (2.0f*informative_content(lcs))/(informative_content(word1.id) + informative_content(word2.id));
}
