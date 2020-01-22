#include "concept_map.hpp"

#include <sstream>

#include <wordnet.hpp>
#include <similarity.hpp>

using namespace std;

int main()
{
	wordnet& wn = wordnet::get_instance("../wordnet/dict/wordnet.db");
	synset s1 (wn.get_id("senate")[0]);
	synset s2 (wn.get_id("law")[0]);
	cout << "    " << similarity::compare_words(s1, s2) << endl << endl;
	string a = "animal\tbla\tdog\nanimal\tbla\thamster\ndog\tbla\tpug\npug\tbla\tcookie";
	//string b = "animal\tbla\tdog\nanimal\tbla\thamster\ndog\tbla\talano";
	string b = "pizza\tbla\tpancake\npancake\t\bla\tbistecca";
	
	stringstream astream(a);
	stringstream bstream(b);
	concept_map cm2 ("/Users/carminemargiotta/Desktop/mappa2.txt");
	//concept_map cm1 ("/Users/carminemargiotta/Desktop/mappa1.txt");
	
	//cout << cm1.similarity(cm2) << endl;
	
	return 0;
}
