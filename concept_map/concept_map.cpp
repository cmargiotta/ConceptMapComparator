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
		
		add_to_corpus(node1);
		add_to_corpus(node2);
	}
	
	build_synsets();
	disambiguate();
}

void concept_map::add_to_corpus(const string& sentence)
{
	stringstream stream (sentence);
	string word;
	std::locale loc;
	
	while(stream >> word)
	{
		for (char& c : word)
		{
			c = std::tolower(c, loc);
		}
		word_corpus.insert(word);
	}
}

void concept_map::build_synsets()
{
	wordnet& wn = wordnet::get_instance();
	
	for (const string& w: word_corpus)
	{
		try
		{
			for (const string& id: wn.get_id(w))
			{
				synset_corpus.emplace_back(id);
				
				id_to_word[id] = w;
			}
		} catch(...)
		{
			continue;
		}
	}
}

void concept_map::disambiguate()
{
	function <float(const synset&, const synset&)> dist = [](const synset& s1, const synset& s2)
	{
		float sim = similarity::compare_words(s1, s2);
		
		/*					
		//Similarity threshold
		if (s1.id != wn.get_entity_id() && s2.id != wn.get_entity_id() && sim <= 0.25f)
			return 1.0f;
		if ((s1.id == wn.get_entity_id() || s2.id == wn.get_entity_id()) && sim <= 0.25f)
			return 0.75f;
			*/
					
		return 1.0f - sim;
	};
	
	function <bool(std::vector<synset>&, const synset&)> termination_condition = [this](std::vector<synset>& clust, const synset& center)
    {		
		map<string, size_t> counters;
		size_t not_in_cluster = 0;
		
		for (synset& s: clust)
		{
			counters[this->id_to_word[s.id]]++;
		}		
		
		for (const string& c: this->word_corpus)
		{
			if (counters[c] == 0)
			{
				not_in_cluster++;
			}
		}
		
		//Removing duplicates
		if ((float) not_in_cluster/this->word_corpus.size() < 0.85f)
		{
			map<string, pair<string, float>> data;
			
			for(const auto& el: clust)
			{
				float dist = similarity::compare_words(el, center);
				string word = this->id_to_word[el.id];
				try
				{
					float old_dist = data.at(word).second;
					
					if (old_dist < dist)
					{
						data[word].first = el.id;
						data[word].second = dist;
					}
				} catch(...)
				{
					data[word].first  = el.id;
					data[word].second = dist;
				}
			}
			
			for (const auto& el: data)
			{
				this->synset_corpus.push_back(el.second.first);
			}

			return true;
		}
		
		return false;
	};
	
	clustering c (synset_corpus, {0, 1, 2}, dist, termination_condition);
	synset_corpus.clear(); 
	c.find_clusters();	
	
	for (const auto& el: synset_corpus)
	{
		this->synset_corpus.push_back(el.id);
		cout << wordnet::get_instance().get_word(el.id) << "   " << el.id << endl;
	}
	cout << endl;
}

concept_map::~concept_map()
{}

float concept_map::similarity(const concept_map& other)
{	
	size_t count = 0;
	float score = 0.0f;
	
	for (auto& s1: synset_corpus)
	{
		float max = 0.0f;
		for (auto& s2: other.synset_corpus)
		{
			float sim = similarity::compare_words(s1, s2);
			if (sim > max)
			{
				max = sim;
			}
		}
		score += max;
		count++;
	}
	
	return score/count;
}
