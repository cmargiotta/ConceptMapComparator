#ifndef WORDNET_HPP
#define WORDNET_HPP

#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <set>
#include <list>

#include <SQLiteCpp/SQLiteCpp.h>

struct synset
{
	std::string id;
	std::map <std::string, size_t> hypernym_path;
	std::map <std::string, size_t> semfield_path;
	size_t hyponym_count;
	
	synset(const std::string& id);
	synset(const synset& other);
	synset() {}
  
	bool operator==(const synset& other) const;
};

class wordnet
{
	friend struct synset;
	private:
		SQLite::Database db;

		std::map<std::string, std::vector<std::string>> words;
		std::map<std::string, std::vector<std::string>> semfields;
		std::map<std::string, std::vector<std::string>> semfield_hierarchy;
		std::map<std::string, std::set<std::string>> hypernyms;
		std::map<std::string, std::set<std::string>> hyponyms;

		std::string entity_id;

		//Private constructor
		wordnet(const std::string& path);

		void add_hyponym(const std::string& start_id, std::string id="");
		void build_tree(const std::string& id, std::map <std::string, size_t>& t, size_t depth = 1);
		void build_semfield_tree(const std::string& id, std::map <std::string, size_t>& t, size_t depth);
		void build_semfield_tree(const std::string& id, std::map <std::string, size_t>& t);
	public:
		//Singleton instance
		static wordnet& get_instance(const std::string& path = "wordnet/dict/wordnet.db")
		{
			static wordnet wb (path);
			return wb;
		}

		~wordnet();

		void hypernym_tree(synset& word);
		void semfield_tree(synset& word);

		std::vector<synset> get_synsets(std::string word);
		std::string get_entity_id();
		size_t get_concept_number();
		std::string get_word(std::string id);
		std::string get_semfield(std::string id);
};

#endif
