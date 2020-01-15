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
	synset(const synset& other);
	synset() {}
	
	bool operator==(const synset& other) const;
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
		
		//Private constructor
		wordnet(const std::string& path);
		
		void add_hyponym(unsigned int start_id, unsigned int id=0);
		void discard_copyright_disclaimer(std::ifstream& file);
		void build_tree(unsigned int id, std::map <unsigned int, unsigned int>& t, unsigned int depth = 1);
	public:
		//Singleton instance
		static wordnet& get_instance(const std::string& path = "wordnet/dict/")
		{
			static wordnet wb (path);
			return wb;
		}
		
		~wordnet();
		
		void 								hypernym_tree(synset& word);
		
		const std::vector<unsigned int>& 	get_id(std::string word);
		unsigned int 						get_hyponym_count(unsigned int word);
		unsigned int						get_entity_id();
		size_t 								get_concept_number();
		std::string 						get_word(unsigned int id);
};

#endif
