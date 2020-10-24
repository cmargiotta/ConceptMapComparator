#include "concept_map.hpp"

#include <fstream>
#include <wordnet.hpp>
#include <sstream>
#include <functional>
#include <algorithm>
#include <set>
#include <locale> 
#include <optional>

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
using std::optional;

concept_map::concept_map(std::string  file_path)
{
	ifstream file (file_path);
	
	*this = concept_map(file);
}

concept_map::concept_map(std::istream& input_stream):
	word_count(0)
{
	string node1, node2, buffer;
	
	set<string> sentences;
	
	while(!input_stream.eof())
	{
		//Reading first node
		getline(input_stream, node1, '\t');
		
		//Reading relation
		getline(input_stream, buffer, '\t');
		
		//Reading second node
		getline(input_stream, node2, '\n');
		
		add_node(node2, add_node(node1, NULL));		
	}
	
	disambiguate();
}

concept_map::node* concept_map::add_node(const std::string& sentence, node* parent)
{
	if (nodes.contains(sentence))
	{
		if (parent != NULL)
		{
			nodes.at(sentence).parent = parent;
			parent->children.emplace_back(&(nodes.at(sentence)));
		}
		
		return &(nodes.at(sentence));
	} else
	{
		nodes.emplace(sentence, parent);
		node* n = &(nodes.at(sentence));
		
		set<string> words = extract_words(sentence);
		for (const string& w: words)
		{
			build_synsets(w);
			n->words.insert(w);
		}
		
		return n;
	}
}

std::set<std::string> concept_map::extract_words(const string& sentence)
{
	set<string> words;
	stringstream stream (sentence);
	string word;
	std::locale loc;
	
	while(stream >> word)
	{
		for (char& c : word)
		{
			c = std::tolower(c, loc);
		}
		words.insert(word);
	}
	
	return words;
}

void concept_map::build_synsets(const std::string& word)
{
	if (word.length() < 3)
		return;
	
	try
	{
		auto s = get_synsets(word);
		
		for (const auto& syn: s)
		{
			synset_corpus.emplace_back(syn);
			
			id_to_word[syn.id] = word;
		}
		
		if (s.size() == 1)
		{
			unique_synsets.emplace_back(s[0]);
		}
		
		word_count++;
	} catch(...)
	{
		return;
	}
}

void concept_map::disambiguate()
{
	function <float(const synset&, const synset&)> dist = [](const synset& s1, const synset& s2)
	{
		float sim = similarity::compare_words(s1, s2);
					
		return 1.0f - sim;
	};
	
	clustering c (synset_corpus, word_count/5, dist);
	
	c.find_clusters();
	
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
	
	for (auto& el: data)
	{
		synset_corpus.push_back(el.second.first);
		
		for (auto& n: nodes)
		{
			if (n.second.words.contains(el.first))
			{
				n.second.synsets.insert(&(*(synset_corpus.end()-1)));
			}
		}
	}

	for (auto& el: unique_synsets)
	{
		synset_corpus.push_back(el);
		
		for (auto& n: nodes)
		{
			if (n.second.words.contains(id_to_word[el.id]))
			{
				n.second.synsets.insert(&(*(synset_corpus.end()-1)));
			}
		}
	}
}

concept_map::~concept_map()
{}

//Corpus-based and Knowledge-based Measures of Text Semantic Similarity (Mihalcea et al. 2006)
float concept_map::similarity(concept_map& other)
{
	
	float sim = 0.0f;
	
	for (auto& n: nodes)
	{
		float s = 0.0f;
		
		for (auto& other_n: other.nodes) {
			float ss = n.second.compare(other_n.second);
			
			if (ss > s)
				s = ss;
		}
		
		sim += s;
	}
	
	return (sim/(nodes.size()-1));
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

concept_map::node::node(node* par):
	parent(par)
{
	if (parent != NULL)
		parent->children.push_back(this);
}

concept_map::node::node(const node& other):
	synsets(other.synsets),
	parent(other.parent),
	children(other.children)
{}

float concept_map::node::compare(node& other) const
{
	float score1 = 0.0f;
	float score2 = 0.0f;
	
	float ic_sum = 0.0f;
	float ic;
	float max = 0.0f;
	
	for (const auto& s1: synsets)
	{
		max = 0.0f;
		
		for (const auto& s2: other.synsets)
		{
			float sim = similarity::compare_words(*s1, *s2);

			if (sim > max)
			{
				max = sim;
			}
		}
		
		ic = similarity::informative_content(*s1);
		score1 += max*ic;
		ic_sum += ic;
	}
	
	if (ic_sum == 0.0f)
		score1 = 0;
	else
		score1/=ic_sum;
		
	ic_sum = 0.0f;
	
	for (auto& s1: other.synsets)
	{
		max = 0.0f;
		for (auto& s2: synsets)
		{
			float sim = similarity::compare_words(*s1, *s2);
			if (sim > max)
			{
				max = sim;
			}		

		}
		ic = similarity::informative_content(*s1);
		score2 += max*ic;
		ic_sum += ic;
	}
	
	if (ic_sum == 0)
		score2 = 0;
	else
		score2/=ic_sum;	
		
	return 0.5f*(score1+score2);
}
