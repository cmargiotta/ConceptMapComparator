#include "similarity.hpp"

#include <vector>
#include <list>
#include <utility>
#include <algorithm>
#include <iostream>

#include "wordnet.hpp"

using namespace std;

float similarity::compare_words(unsigned int word1, unsigned int word2)
{
	static wordnet wn("wordnet/dict/");
	
	tree<unsigned int> path1 = wn.hypernym_tree(word1);
	tree<unsigned int> path2 = wn.hypernym_tree(word2);
	
	for (auto i: path1.path)
		cout << wn.get_word(i.first) << "   " << i.second << endl;
		cout << endl;cout << endl;
	for (auto i: path2.path)
		cout << wn.get_word(i.first) << "   " << i.second << endl;
		cout << endl;cout << endl;
	
	std::map<unsigned int, unsigned int> intersection;
	std::set_intersection  (path1.path.begin(), 
							path1.path.end(), 
							path2.path.begin(), 
							path2.path.end(), 
							inserter (intersection, intersection.begin()),
		[](auto& p1, auto& p2)
        {
			 return p1.first < p2.first;
		});
		
	auto best_common_parent = *min_element(intersection.begin(), intersection.end(), [&path1, &path2](auto el1, auto el2)
	{
		return (path1.path[el1.first]+path2.path[el1.first]) < (path1.path[el2.first]+path2.path[el2.first]);
	});
	
	for (auto i: intersection)
		cout << wn.get_word(i.first) << endl;
	cout << endl;
	
	cout << wn.get_word(best_common_parent.first) << endl;
	
	return 0.0f;
}
