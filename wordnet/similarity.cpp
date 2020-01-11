#include <algorithm>

#include "similarity.hpp"

static bool similarity::is_sentence(const std::string& string)
{
	//A string without spaces is a word
	return std::find(string.begin(), string.end(), ' ') != string.end();
}

static float similarity::compare(const std::string& string1, const std::string& string2)
{
	if (is_sentence(string1) || is_sentence(string2)
	{
		return compare_sentences(string1, string2);
	}
	
	return compare_words(string1, string2);
}

static float similarity::compare_words(const std::string& word1, const std::string& word2)
{
	
}

static float similarity::compare_sentences(const std::string& sentence1, const std::string& sentence2)
{
	
}
