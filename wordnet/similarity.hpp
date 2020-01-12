#include <string>

#include "wordnet.hpp"

class similarity
{
	private:
		static unsigned int LCS(const synset& word1, const synset& word2);
		static float informative_content(unsigned int word);
	public:
		static float compare_words(const synset& word1, const synset& word2);
};
