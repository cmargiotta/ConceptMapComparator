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

wordnet::wordnet(const std::string& path):
	noun_index((path.back() == '/') ? (path + "index.noun") : (path + "/index.noun")),
	noun_data((path.back() == '/') ? (path + "data.noun") : (path + "/data.noun"))
{
	discard_copyright_disclaimer(noun_index);
	discard_copyright_disclaimer(noun_data);
	
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
		noun_index >> pointers_kind_count;
		
		for (size_t i = 0; i < pointers_kind_count; i++)
		{
			noun_index >> pointer_type;
			if (pointer_type[0] == '@')
			{
				hypernym_present = true;
			}
		}
		
		//If there are no hypernyms, the word can be discarded
		if (!hypernym_present)
		{
			//Discarding the remaining line
			noun_index.ignore(1024, '\n');
			continue;
		}
		
		noun_index >> synset_count;
		noun_index >> tagsense_count;
		
		for (size_t i = 0; i < synset_count; i++)
		{
			noun_index >> id;
			words[lemma].push_back(id);
		}
	}
	
	cout << "Size" << words.size() << endl;
	
	int buf;
	unsigned short word_count, pointer_count;
	std::string word;
	//Parsing data from data.noun
	for (std::pair<const std::string, std::vector<unsigned int>>& voice: words)
	{
		for (unsigned int syn_id: voice.second) 
		{
			cout << "Parsing for " << voice.first << "  " << (streampos)syn_id << endl;
			noun_data.seekg(syn_id);
			cout << noun_data.tellg() << endl;
			if (noun_data.tellg() < 0)
				exit(0);
			
			noun_data >> line;
			cout << line << endl;
			noun_data >> buf >> category;
			noun_data >> word_count;
			cout << word_count << endl;
			
			for (size_t i = 0; i < word_count; i++)
			{
				noun_data >> word;
				cout << word << endl;
				noun_data >> buf;
			}
			
			noun_data >> pointer_count;
			for (size_t i = 0; i < pointer_count; i++)
			{
				noun_data >> pointer_type;
				noun_data >> id;
				noun_data >> category;
				noun_data >> buf;
				
				//We are interested only in hypernyms (nouns)
				if (pointer_type[0] == '@' && category == 'n')
				{
					hypernyms[syn_id] = id;
				}
			}
		}
	} 
}
