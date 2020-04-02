#ifndef CONCEPT_MAP_HPP
#define CONCEPT_MAP_HPP

#include <vector>
#include <string>
#include <set>
#include <optional>
#include <iostream>
#include <map>

#include <wordnet.hpp>

class concept_map
{
	private:
		std::map<std::string, std::vector<std::string>> 	adjancencies;
		std::map<std::string, std::string>					id_to_word;
		std::set<std::string> 								word_corpus;
		std::vector<synset> 								synset_corpus;
		std::string 										data;
		
		void add_to_corpus(const std::string& sentence);
		void disambiguate();
		void build_synsets();
	
	public:
		concept_map(std::string  file_path);
		concept_map(std::istream& input_stream);
		
		~concept_map();
		
		void 	add_child(std::string parent, std::string data);
		float 	similarity(const concept_map& other);
		
		std::vector<std::string>   get_keywords();
		const std::vector<synset>& get_keywords_synsets();
};

#endif
