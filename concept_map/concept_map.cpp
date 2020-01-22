#include "concept_map.hpp"

#include <fstream>
#include <wordnet.hpp>
#include <sstream>
#include <functional>
#include <algorithm>
#include <set>
#include <locale> 

#include <similarity.hpp>
#include <wordnet.hpp>
#include <clustering.hpp>

using std::ifstream;
using std::string;
using std::stoi;
using std::getline;
using std::optional;
using std::stringstream;
using std::set;
using std::function;
using std::find;

using namespace std;

concept_map::concept_map(std::string  file_path)
{
	ifstream file (file_path);
	
	*this = concept_map(file);
}

concept_map::concept_map(std::istream& input_stream)
{
	string node1, node2, buffer;
	
	while(!input_stream.eof())
	{
		//Reading first node
		getline(input_stream, node1, '\t');
		
		//Reading relation
		getline(input_stream, buffer, '\t');
		
		//Reading second node
		getline(input_stream, node2);
		
		//Saving relation
		adjancencies[node1].push_back(node2);
		
		whole_map_corpus.insert(node1);
		whole_map_corpus.insert(node2);
	}
	
	build_corpus();
	disambiguate();
}

void concept_map::build_corpus()
{
	wordnet& wn = wordnet::get_instance();
	synset_corpus.emplace_back(wn.get_entity_id());
	
	for (const string& node: whole_map_corpus)
	{
		stringstream stream (node);
		string word;
		std::locale loc;
		
		while(stream >> word)
		{
			try
			{
				if (word_corpus.find(word) == word_corpus.end())
				{
					for(auto& c : word)
					{
						c = std::tolower(c, loc);
					}
					word_corpus.insert(word);
					
					for (const string& id: wn.get_id(word))
					{
						synset_corpus.emplace_back(id);
						
						id_to_map_word[id] = word;
					}
				}
			} catch(...)
			{
				continue;
			}
		}
	}
	
	cout << endl;
}

void concept_map::disambiguate()
{
	function <float(const synset&, const synset&)> dist = [](const synset& s1, const synset& s2)
	{
		wordnet& wn = wordnet::get_instance();
		float sim = similarity::compare_words(s1, s2);
							
		//Similarity threshold
		if (s1.id != wn.get_entity_id() && s2.id != wn.get_entity_id() && sim <= 0.35f)
			sim = 0.0f;
		if ((s1.id == wn.get_entity_id() || s2.id != wn.get_entity_id()) && sim <= 0.35f)
			sim = 0.35f;
					
		return 1.0f - sim;
	};
	
	function <bool(std::vector<synset>&, const synset&)> termination_condition = [this](std::vector<synset>& clust, const synset& center)
    {
		wordnet& wn = wordnet::get_instance();
		
		map<string, size_t> counters;
		size_t not_in_cluster = 0;
		
		cout << "Center " << wn.get_word(center.id) << endl;
		for (synset& s: clust)
		{
			cout << wn.get_word(s.id) << "   " << similarity::compare_words(s, center) << endl;
			counters[this->id_to_map_word[s.id]]++;
		}
		cout << endl;
		
		
		for (const string& c: this->word_corpus)
		{
			if (counters[c] == 0)
			{
				not_in_cluster++;
			}
		}
		cout << (float) not_in_cluster/this->word_corpus.size() << endl << endl;
		return (float) not_in_cluster/this->word_corpus.size() < 0.7f && center.id != wn.get_entity_id();
	};
	
	clustering c (synset_corpus, {0, 1, 2}, dist, termination_condition); 
	c.find_clusters();
	
	synset_corpus.clear();
	
	for(const auto& c: c.get_clusters()[0].elements)
	{
		synset_corpus.push_back(c);
		cout << wordnet::get_instance().get_word(c.id) << endl;
	}
	
	cout << endl;
}

concept_map::~concept_map()
{}

float concept_map::similarity(const concept_map& other)
{
	//Discarding duplicated words
	struct synsetcomp {
		bool operator() (const synset& lhs, const synset& rhs) const
		{return (lhs.id < rhs.id);}
	};
	set<synset, synsetcomp> corpus_set;
	vector<bool> a, b;
	float max = 0.0f;
	float sim = 0.0f;
	float dist;
	
	for (auto& s: synset_corpus)
	{
		corpus_set.insert(s);
	}
	for (auto& s: other.synset_corpus)
	{
		corpus_set.insert(s);
	}
	
	vector<synset> corpus (corpus_set.begin(), corpus_set.end());
	
	for (auto& s: corpus)
	{
		a.push_back(find(synset_corpus.begin(), synset_corpus.end(), s) != synset_corpus.end());
		b.push_back(find(other.synset_corpus.begin(), other.synset_corpus.end(), s) != other.synset_corpus.end());
	}
	
	for (size_t i = 0; i < corpus.size(); i++)
	{
		for (size_t j = 0; j < corpus.size(); j++)
		{
			dist = similarity::compare_words(corpus[i], corpus[j]);
			dist = (dist <= 0.35f) ? 0.0f : dist;
			
			if (b[i])
			{
				sim += a[j]*dist;
				max += a[j];
			}
		}
	}
			
	return sim/max;
}
