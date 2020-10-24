#include "wordnet.hpp"

#include <utility>
#include <sstream>

#include "db_initializer.hpp"

using namespace std;

/*** SYNSET STRUCT ***/

synset::synset(const std::string& id) 
{
	auto& wn = wordnet::get_instance();
	
	this->id = id;
	hyponym_count = wn.get_hyponyms_number(id);
	hypernym_path[id] = 0;
	
	wn.hypernym_tree(*this);
		
	wn.semfield_tree(*this);
}

synset::synset(const synset& other):
	id(other.id), 
	hypernym_path(other.hypernym_path.begin(), other.hypernym_path.end()),
	semfield_path(other.semfield_path.begin(), other.semfield_path.end()),
	hyponym_count(other.hyponym_count)
{}

bool synset::operator==(const synset& other) const
{
	return other.id == id;
}

/*********************/

wordnet::~wordnet()
{}

wordnet::wordnet(const std::string& path):
	db(path, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE)
{
	assert(db.tableExists("nouns"));
	assert(db.tableExists("hypernyms"));
	
	if (!db.tableExists("hyponyms_count"))
	{
		db_initializer(db);
	}
	
	//Reading semfield hierarchy
	SQLite::Statement   query1(db, "SELECT * FROM semfield_hierarchy");
	while (query1.executeStep())
	{
		const char* field1 = query1.getColumn(1);
		const char* field2 = query1.getColumn(3);
				
		semfield_hierarchy[field1].push_back(field2);
	}
	
	SQLite::Statement   query2(db, "SELECT * FROM entity_id");
	while (query2.executeStep())
	{
		const char* field1 = query2.getColumn(0);
				
		entity_id = field1;
	}
	
	concept_number = get_hyponyms_number(entity_id);
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

void wordnet::build_tree(const string& id, map <string, size_t>& t, size_t depth)
{
	for (const string& child_id: get_hypernyms(id))
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

void wordnet::build_semfield_tree(const string& id, map <string, size_t>& t, size_t depth)
{
	for (const string& child_id: semfield_hierarchy[id])
	{
		//Selecting min depth for every node 
		size_t old_depth = t[child_id];
		t[child_id] = (old_depth != 0) ? min(old_depth, depth) : depth;
		
		build_tree(child_id, t, depth + 1);
	}
}

void wordnet::build_semfield_tree(const string& id, map <string, size_t>& t)
{
	for (const string& semfield: get_semfields(id))
	{		
		t[semfield] = 1;
		build_semfield_tree(semfield, t, 2);
	}
}

void wordnet::hypernym_tree(synset& word)
{
	build_tree(word.id, word.hypernym_path);
}

void wordnet::semfield_tree(synset& word)
{
	build_semfield_tree(word.id, word.semfield_path);
}

std::vector<synset> wordnet::get_synsets(std::string word)
{
	std::vector<synset> synsets;
	
	try
	{
		words.at(word);
	}
	catch(...)
	{
		SQLite::Statement query(db, "SELECT * FROM nouns WHERE lemma=?");
		query.bind(1, word);
		
		while (query.executeStep())
		{
			words[word].push_back(query.getColumn(1));
		}
		
		words.at(word);
	}
	
	synsets.reserve(words[word].size());

	for (auto& id: words[word])
	{
		synsets.emplace_back(id);
	}

	return synsets;
}

size_t wordnet::get_concept_number()
{
	//words.size() is not correct, for a synset we can have more than one word (probably one per language)
	return concept_number;
}

string wordnet::get_entity_id()
{
	return entity_id;
}

const std::set<std::string>& wordnet::get_hypernyms(const std::string& word_id)
{
	try
	{
		return hypernyms.at(word_id);
	} catch(...)
	{
		SQLite::Statement query(db, "SELECT * FROM hypernyms WHERE id_source=?");
		query.bind(1, word_id);
		
		while (query.executeStep())
		{
			hypernyms[word_id].insert(query.getColumn(1));
		}
		
		return hypernyms[word_id];
	}
}

const std::vector<std::string>& wordnet::get_semfields(const std::string& word_id)
{
	try
	{
		return semfields.at(word_id);
	} catch(...)
	{
		SQLite::Statement query(db, "SELECT * FROM semfields WHERE word_id=?");
		query.bind(1, word_id);
		
		while (query.executeStep())
		{
			semfields[word_id].push_back(query.getColumn(1));
		}
		
		return semfields[word_id];
	}
}

size_t wordnet::get_hyponyms_number(const std::string& word_id)
{
	try
	{
		return hyponyms_number.at(word_id);
	} catch(...)
	{
		SQLite::Statement query(db, "SELECT count FROM hyponyms_count WHERE word_id=?");
		query.bind(1, word_id);
		
		while (query.executeStep())
		{
			hyponyms_number[word_id] = atol(query.getColumn(0));
		}
		
		return hyponyms_number[word_id];
	}
}
