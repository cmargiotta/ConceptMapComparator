#ifndef WORDNET_HPP
#define WORDNET_HPP

#include <string>
#include <map>
#include <fstream>
#include <vector>

class wordnet
{
	public:
		std::ifstream					noun_index;
		std::ifstream					noun_data;
		
		std::map<std::string, std::vector<unsigned int>>	words;
		std::map<unsigned int, unsigned int> 				hypernyms;
		
		void discard_copyright_disclaimer(std::ifstream& file);
	public:
		wordnet(const std::string& path);
};

#endif
