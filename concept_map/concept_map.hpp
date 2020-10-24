#ifndef CONCEPT_MAP_HPP
#define CONCEPT_MAP_HPP

#include <vector>
#include <string>
#include <set>
#include <iostream>
#include <map>

#include <wordnet.hpp>

class concept_map
{
	private:
		struct node
		{
			std::set<synset*> synsets;
			std::set<std::string> words;
			node* parent;
			std::vector<node*> children;
			
			node(node* parent);
			node(const node& other);

			float compare(node& other) const;
		};
	
		std::map<std::string, node>				nodes;
		std::map<std::string, std::string>		id_to_word;
		std::vector<synset>						synset_corpus;
		//No need to disambiguate those
		std::vector<synset>						unique_synsets;
		
		size_t word_count;
		
		std::set<std::string> extract_words(const std::string& sentence);
		void disambiguate();
		node* add_node(const std::string& words, node* parent);
		void build_synsets(const std::string& word);
	
	public:
		concept_map(std::string  file_path);
		concept_map(std::istream& input_stream);
		
		~concept_map();
		
		void 	add_child(std::string parent, std::string data);
		float 	similarity(concept_map& other);
		
		std::vector<std::string>   get_keywords();
		const std::vector<synset>& get_keywords_synsets();
};

#endif
