#include <catch.hpp>

#include <string>
#include <iostream>

#include <wordnet.hpp>
#include <similarity.hpp>
#include <word.hpp>

using namespace std;

SCENARIO("Comparing two words")
{
	GIVEN("Two totally different words")
	{
		wordnet::get_instance("../wordnet/dict/wordnet.db");
		
		string w1 ("dog");
		string w2 ("heater");
		
		synset s1 (get_synsets(w1)[0]);
		synset s2 (get_synsets(w2)[0]);
				
		WHEN("The simimilarity between them is computed")
		{
			float s = similarity::compare_words(s1, s2);
			
			THEN("The similarity is low")
			{
				REQUIRE(s <= 0.1f);
			}
		}
	}
	
	GIVEN("Two similar words")
	{
		string w1 ("dog");
		string w2 ("pug");
		
		synset s1 = get_synsets(w1)[0];
		synset s2 = get_synsets(w2)[0];
		
		
		WHEN("The simimilarity between them is computed")
		{
			float s = similarity::compare_words(s1, s2);
			
			THEN("The similarity is high")
			{
				REQUIRE(s >= 0.8f);
			}
		}
	}
	
	GIVEN("Two similar words, but plural")
	{
		string w1 ("dogs");
		string w2 ("pugs");
		
		synset s1 = get_synsets(w1)[0];
		synset s2 = get_synsets(w2)[0];
		
		
		WHEN("The simimilarity between them is computed")
		{
			float s = similarity::compare_words(s1, s2);
			
			THEN("The similarity is high")
			{
				REQUIRE(s >= 0.8f);
			}
		}
	}
	
	GIVEN("Two equal words")
	{
		string w1 ("dog");
		
		synset s1 = get_synsets(w1)[0];
		
		WHEN("The simimilarity between them is computed")
		{
			float s = similarity::compare_words(s1, s1);
			
			THEN("The similarity is 1")
			{
				REQUIRE(s == 1);
			}
		}
	}
	
	GIVEN("Two words only in the same category (like lawyer and tribunal)")
	{
		string w1 ("lawyer");
		string w2 ("tribunal");
		
		synset s1 = get_synsets(w1)[0];
		synset s2 = get_synsets(w2)[0];
		
		WHEN("The similarity between them is computed")
		{
			float s = similarity::compare_words(s1, s2);
			
			THEN("The similarity is not low")
			{
				REQUIRE(s >= 0.5);
			}
		}
	}
}
