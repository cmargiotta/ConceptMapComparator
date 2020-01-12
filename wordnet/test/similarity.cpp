#include <catch.hpp>

#include <string>

#include <wordnet.hpp>
#include <similarity.hpp>

using namespace std;

SCENARIO("Comparing two words")
{
	GIVEN("Two totally different synsets")
	{
		wordnet& wn = wordnet::get_instance("../wordnet/dict");
		
		string w1 ("dog");
		string w2 ("heater");
		
		synset s1 (wn.get_id(w1)[0]);
		synset s2 (wn.get_id(w2)[0]);
		
		
		WHEN("The simimilarity between them is computed")
		{
			float s = similarity::compare_words(s1, s2);
			
			THEN("The similarity is low")
			{
				REQUIRE(s <= 0.3);
			}
		}
	}
	
	GIVEN("Two similar synsets")
	{
		wordnet& wn = wordnet::get_instance();
		
		string w1 ("dog");
		string w2 ("pug");
		
		synset s1 (wn.get_id(w1)[0]);
		synset s2 (wn.get_id(w2)[0]);
		
		
		WHEN("The simimilarity between them is computed")
		{
			float s = similarity::compare_words(s1, s2);
			
			THEN("The similarity is high")
			{
				REQUIRE(s >= 0.7);
			}
		}
	}
	
	GIVEN("Two equal synsets")
	{
		wordnet& wn = wordnet::get_instance();
		
		string w1 ("dog");
		
		synset s1 (wn.get_id(w1)[0]);
		
		WHEN("The simimilarity between them is computed")
		{
			float s = similarity::compare_words(s1, s1);
			
			THEN("The similarity is 1")
			{
				REQUIRE(s == 1);
			}
		}
	}
}
