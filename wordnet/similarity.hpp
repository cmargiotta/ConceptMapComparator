#include <string>

#include "wordnet.hpp"

class similarity
{
	private:
		static synset LCS(const synset& word1, const synset& word2);
		static std::string nearest_common_semfield(const synset& word1, const synset& word2);
		static float informative_content(const synset& syn);
	public:
		static float compare_words(const synset& word1, const synset& word2);
};
