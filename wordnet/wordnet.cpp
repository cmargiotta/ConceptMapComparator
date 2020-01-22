#include "wordnet.hpp"

#include <utility>
#include <iostream>

using namespace std;

using std::cout;

wordnet::~wordnet()
{}

void wordnet::add_hyponym(const string& start_id, string id)
{
	for (auto i: hypernyms[(id=="") ? start_id : id])
	{
		hyponyms[i].insert(start_id);
		add_hyponym(start_id, i);
	}
}

wordnet::wordnet(const std::string& path):
	db(path)
{
	assert(db.tableExists("nouns"));
	assert(db.tableExists("hypernyms"));
	
	SQLite::Statement   query1(db, "SELECT lemma, synset FROM nouns");
	while (query1.executeStep())
	{
		const char* lemma  		= query1.getColumn(0);
		const char* synset_id 	= query1.getColumn(1);
				
		words[lemma].push_back(synset_id);
	}
	
	SQLite::Statement   query2(db, "SELECT * FROM hypernyms");
	
	while (query2.executeStep())
	{
		const char* syn1 = query2.getColumn(0);
		const char* syn2 = query2.getColumn(1);
				
		hypernyms[syn1].insert(syn2);
		add_hyponym(syn1);
	}
	
	entity_id = words["entity"][0];
	
	//Adding entity as hypernym of every node without hypernyms
	for (auto& w: words)
	{
		for (auto& s: w.second)
		{
			if (hypernyms[s].size() == 0)
			{
				hypernyms[s].insert(entity_id);
				hyponyms[entity_id].insert(s);
			}
		}
	}
}

std::string wordnet::get_word(string id)
{
	SQLite::Statement query(db, "SELECT lemma FROM nouns WHERE synset=?");
	query.bind(1, id);
	
	while (query.executeStep())
    {
		return std::string(query.getColumn(0));
	}
	
	return "";
}

void wordnet::build_tree(const string& id, std::map <string, size_t>& t, size_t depth)
{
	for (const string& child_id: hypernyms[id])
	{
		//Selecting min depth for every node 
		size_t old_depth = t[child_id];
		t[child_id] = (old_depth != 0) ? min(old_depth, depth) : depth;
		
		if (child_id != entity_id)
		{
			build_tree(child_id, t, depth + 1);
		}
	}
}

void wordnet::hypernym_tree(synset& word)
{
	build_tree(word.id, word.hypernym_path);
}

const std::vector<std::string>& wordnet::get_id(std::string word)
{
	return words.at(word);
}

size_t wordnet::get_hyponym_count(string word)
{
	return hyponyms[word].size();
}

size_t wordnet::get_concept_number()
{
	//words.size() is not correct, for a synset we can have more than one word (probably one per language)
	return hyponyms[entity_id].size();
}

string wordnet::get_entity_id()
{
	return entity_id;
}

synset::synset(string id)
{
	this->id = id;
	wordnet& wb = wordnet::get_instance();
	
	hypernym_path[id] = 0;
	
	wb.hypernym_tree(*this);
}

synset::synset(const synset& other):
	id(other.id), 
	hypernym_path(other.hypernym_path.begin(), other.hypernym_path.end())
{}

bool synset::operator==(const synset& other) const
{
	return other.id == id;
}
