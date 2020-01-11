#include <string>

#include <wnb/core/wordnet.hh>
#include <wnb/nltk_similarity.hh>

class similarity
{
	private:
		static bool is_sentence(const std::string& string);
		static float compare_words(const std::string& word1, const std::string& word2);
		static float compare_sentences(const std::string& sentence1, const std::string& sentence2);
	public:
		//A string is considered as a word if there are no spaces
		static float compare(const std::string& string1, const std::string& string2);
}
