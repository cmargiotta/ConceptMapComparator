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
	std::map<unsigned int, unsigned int> intersection;
	std::set_intersection  (word1.hypernym_path.begin(), 
							word1.hypernym_path.end(), 
							word2.hypernym_path.begin(), 
							word2.hypernym_path.end(), 
							inserter (intersection, intersection.begin()),
							[](auto& p1, auto& p2)
							{
								 return p1.first < p2.first;
							});
		
	unsigned int best_common_parent = min_element(intersection.begin(), intersection.end(), [&word1, &word2](auto el1, auto el2)
	{
		auto e1 = el1.first;
		auto e2 = el2.first;
		return (word1.hypernym_path.at(e1)+word2.hypernym_path.at(e1)) < (word1.hypernym_path.at(e2)+word2.hypernym_path.at(e2));
	})->first;
	
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
