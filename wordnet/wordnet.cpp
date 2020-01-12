#include "wordnet.hpp"

#include <utility>
#include <iostream>

using namespace std;
void wordnet::discard_copyright_disclaimer(std::ifstream& file)
{
	char buffer;
	file.get(buffer);

	while(buffer == ' ')
	{
		file.ignore(1024, '\n');
		file.get(buffer);
	}
	
	file.seekg(-1, file.cur);
}

void wordnet::add_hyponym(unsigned int start_id, unsigned int id)
{
	for (auto i: hypernyms[(id==0) ? start_id : id])
	{
		hyponyms[i].insert(start_id);
		add_hyponym(start_id, i);
	}
}

wordnet::wordnet(const std::string& path):
	noun_index((path.back() == '/') ? (path + "index.noun") : (path + "/index.noun"), std::ifstream::binary),
	noun_data((path.back() == '/') ? (path + "data.noun") : (path + "/data.noun"), std::ifstream::binary)
{
	discard_copyright_disclaimer(noun_index);
	discard_copyright_disclaimer(noun_data);
	
	//Reading entity ID
	noun_data >> std::dec >> entity_id;
	
	std::string lemma, line, pointer_type;
	char category;
	unsigned short synset_count, pointers_kind_count, tagsense_count;
	bool hypernym_present;
	unsigned int id;
	
	
	//Parsing data from index.noun
	while (noun_index >> lemma)
	{
		hypernym_present = false;
		noun_index >> category;
		noun_index >> synset_count;
		noun_index >> std::dec >> pointers_kind_count;
		
		for (size_t i = 0; i < pointers_kind_count; i++)
		{
			noun_index >> pointer_type;
			if (pointer_type[0] == '@')
			{
				hypernym_present = true;
			}
		}
		
		noun_index >> std::dec >> synset_count;
		noun_index >> tagsense_count;
		
		for (size_t i = 0; i < synset_count; i++)
		{
			noun_index >> id;
			words[lemma].push_back(id);
		}
	}
		
	int buf;
	unsigned short word_count, pointer_count;
	std::string word;
	//Parsing data from data.noun
	for (std::pair<const std::string, std::vector<unsigned int>>& voice: words)
	{
		for (unsigned int syn_id: voice.second) 
		{
			noun_data.seekg(syn_id);
			
			noun_data >> line;
			noun_data >> buf >> category;
			noun_data >> std::hex >> word_count;
			
			for (size_t i = 0; i < word_count; i++)
			{
				noun_data >> word;
				noun_data >> buf;
			}
			
			noun_data >> std::dec >> pointer_count;

			for (size_t i = 0; i < pointer_count; i++)
			{
				noun_data >> pointer_type;
				noun_data >> std::dec >> id;
				noun_data >> category;
				noun_data >> std::hex >> buf;
								
				//We are interested only in hypernyms (nouns)
				if (pointer_type[0] == '@' && category == 'n')
				{
					hypernyms[syn_id].insert(id);
				}
			}
		}
	}
	
	for (auto& i: words)
	{
		for (auto w: i.second)
		{
			add_hyponym(w);
		}
	}
}

std::string wordnet::get_word(unsigned int id)
{
	noun_data.seekg(id);
	
	std::string word;
	
	noun_data >> word >> word >> word >> word >> word;
	
	return word;
}

void build_tree(unsigned int id, std::map <unsigned int, unsigned int>& t, std::map<unsigned int, std::set<unsigned int>>& hypernyms, unsigned int entity, unsigned int depth = 1)
{
	for (unsigned int child_id: hypernyms[id])
	{
		unsigned int old_depth = t[child_id];

		t[child_id] = (old_depth != 0) ? min(old_depth, depth) : depth;
		
		if (child_id != entity)
		{
			build_tree(child_id, t, hypernyms, entity, depth + 1);
		}
	}
}

void wordnet::hypernym_tree(synset& word)
{
	build_tree(word.id, word.hypernym_path, hypernyms, entity_id);
}

const std::vector<unsigned int>& wordnet::get_id(std::string& word)
{
	return words[word];
}

unsigned int wordnet::get_hyponym_count(unsigned int word)
{
	return hyponyms[word].size();
}

unsigned int wordnet::get_concept_number()
{
	return words.size();
}

synset::synset(unsigned int id)
{
	this->id = id;
	wordnet& wb = wordnet::get_instance();
	
	hypernym_path[id] = 0;
	
	wb.hypernym_tree(*this);
}
