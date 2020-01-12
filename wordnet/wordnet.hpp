#ifndef WORDNET_HPP
#define WORDNET_HPP

#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <set>
#include <list>

template <typename T>
struct tree
{
	T root;
	//Node - min depth
	std::map <T, unsigned int> path;
	
	tree(T r): root(r) {path[r] = 0;}
	tree() {}
};

struct synset
{
	std::string string;
	unsigned int id;
	tree<unsigned int> path_tree;
	
	synset(std::string& s): string(s) 
	{
		auto wb = wordnet::get_instance();
		
		id = wb.get_id(string);
		path_tree.root = id;
		path_tree.path[id] = 0;
	}
};

class wordnet
{
	private:
		std::ifstream										noun_index;
		std::ifstream										noun_data;
		
		unsigned int										entity_id;
		
		std::map<std::string, std::vector<unsigned int>>	words;
		std::map<unsigned int, std::set<unsigned int>> 		hypernyms;
		
		
		wordnet(const std::string& path);
		void discard_copyright_disclaimer(std::ifstream& file);
	public:
		static wordnet& get_instance(const std::string& path = "wordnet/dict/")
		{
			static wordnet wb (path);
			return wb;
		}
		std::string get_word(unsigned int id);
		unsigned int get_id(std::string& word);
		void hypernym_tree(synset& word, tree<unsigned int>& t);
};

#endif
