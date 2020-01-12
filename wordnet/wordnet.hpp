#ifndef WORDNET_HPP
#define WORDNET_HPP

#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <set>
#include <list>

struct synset
{
	unsigned int id;
	std::map <unsigned int, unsigned int> hypernym_path;
	
	synset(unsigned int id);
};

class wordnet
{
	private:
		std::ifstream										noun_index;
		std::ifstream										noun_data;
		
		unsigned int										entity_id;
		
		std::map<std::string, std::vector<unsigned int>>	words;
		std::map<unsigned int, std::set<unsigned int>> 		hypernyms;
		std::map<unsigned int, std::set<unsigned int>>		hyponyms;
		
		void add_hyponym(unsigned int start_id, unsigned int id=0);
		wordnet(const std::string& path);
		void discard_copyright_disclaimer(std::ifstream& file);
	public:
		static wordnet& get_instance(const std::string& path = "wordnet/dict/")
		{
			static wordnet wb (path);
			return wb;
		}
		std::string get_word(unsigned int id);
		const std::vector<unsigned int>& get_id(std::string& word);
		void hypernym_tree(synset& word);
		unsigned int get_hyponym_count(unsigned int word);
		unsigned int get_concept_number();
};

#endif
