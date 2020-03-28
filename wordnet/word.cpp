#include "word.hpp"

#include <word_to_singular.hpp>

using std::vector;
using std::string;


#include <iostream>
using namespace std;
vector<synset> get_synsets(string word)
{
	wordnet& wn = wordnet::get_instance();
	
	try
	{
		cout << word << endl;
		return wn.get_synsets(word);
	}
	catch(...)
	{
		string singular = get_singular(word);
		return wn.get_synsets(singular);
	}
}
