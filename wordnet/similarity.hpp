#include <string>

#include "wordnet.hpp"

class similarity
{
	private:
		static std::string LCS(const synset& word1, const synset& word2);
		static float informative_content(std::string word);
	public:
		static float compare_words(const synset& word1, const synset& word2);
};
