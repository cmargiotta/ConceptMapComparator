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
#include <word.hpp>

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
	for (const string& w: word_corpus)
	{
		try
		{
			for (const auto& syn: get_synsets(w))
			{
				synset_corpus.emplace_back(syn);
				
				id_to_word[syn.id] = w;
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
					
		return 1.0f - sim;
	};
	
	clustering c (synset_corpus, word_corpus.size()/5, dist);
	c.find_clusters();
	
	//Reading clusters
	synset_corpus.clear(); 

	//We only need one synset per word, keeping only the nearest (to the centroid) one
	map<string, pair<synset, float>> data;
	for (auto& clust: c.get_clusters())
	{
		for (size_t i = 0; i < clust.elements.size(); i++)
		{
			const synset& el = clust.elements[i];
			float dist = clust.distances[i];
			
			string word = id_to_word[el.id];
			try
			{
				float old_dist = data.at(word).second;
				
				if (old_dist > dist)
				{
					data[word].first = el.id;
					data[word].second = dist;
				}
			} catch(...)
			{
				if (dist <= 0.95f)
				{
					data[word].first  = el;
					data[word].second = dist;
				}
			}
		}
	}
	for (const auto& el: data)
	{
		synset_corpus.push_back(el.second.first);
	}
}

concept_map::~concept_map()
{}

//Corpus-based and Knowledge-based Measuresof Text Semantic Similarity (Mihalcea et al. 2006)
float concept_map::similarity(const concept_map& other)
{	
	float score1 = 0.0f;
	float score2 = 0.0f;
	
	float ic_sum = 0.0f;
	
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
		score1 += max;
		ic_sum += similarity::informative_content(s1);
	}
	
	score1/=ic_sum;
	
	ic_sum = 0.0f;
	
	for (auto& s1: other.synset_corpus)
	{
		float max = 0.0f;
		for (auto& s2: synset_corpus)
		{
			float sim = similarity::compare_words(s1, s2);
			if (sim > max)
			{
				max = sim;
			}
		}
		score2 += max;
		ic_sum += similarity::informative_content(s1);
	}
	
	score2/=ic_sum;
	
	return 0.5f*(score1+score2);
}

vector<string> concept_map::get_keywords()
{
	vector<string> keywords;
	keywords.reserve(synset_corpus.size());
	
	for (synset& s: synset_corpus)
	{
		keywords.push_back(id_to_word[s.id]);
	}
	
	return keywords;
}

const std::vector<synset>& concept_map::get_keywords_synsets()
{
	return synset_corpus;
}
