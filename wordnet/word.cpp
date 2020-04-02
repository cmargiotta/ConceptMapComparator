#include "word.hpp"

#include <word_to_singular.hpp>

#include <locale>
#include <algorithm>

using std::vector;
using std::string;


#include <iostream>
using namespace std;
vector<synset> get_synsets(string word)
{
	wordnet& wn = wordnet::get_instance();
	
	std::transform(word.begin(), word.end(), word.begin(), ::tolower);
	
	try
	{
		return wn.get_synsets(word);
	}
	catch(...)
	{
		string singular = get_singular(word);
		return wn.get_synsets(singular);
	}
}
