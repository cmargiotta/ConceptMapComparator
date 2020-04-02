#include <catch.hpp>

#include <string>
#include <vector>
#include <sstream>

#include <concept_map.hpp>

using std::string;
using std::stringstream;
using std::vector;

using Catch::Matchers::Contains; 
using Catch::Matchers::Floating::WithinAbsMatcher; 

SCENARIO("Concept map keywords extraction")
{
	wordnet::get_instance("../wordnet/dict/wordnet.db");
	
	GIVEN("A concept map about roman empire")
	{
		string map = "The Roman Empire\tincluded\tLarge territorial holdings around the Mediterranean Sea\
					  Large territorial holdings around the Mediterranean Sea\truled by\tEmperors\
					  The Roman Empire\twas distinguished\tAlso for its accomplishments in intellectual endeavours";
		stringstream mapstream(map);
		
		concept_map cm (mapstream);
		
		WHEN("Its keywords are computed")
		{
			vector<string> expected = {"roman", "empire", "emperors"};
			
			THEN("They are correct")
			{
				REQUIRE_THAT(cm.get_keywords(), Contains(expected));
			}
		}
	}
}

SCENARIO("Concept map comparison")
{
	wordnet::get_instance("../wordnet/dict/wordnet.db");
	
	GIVEN("Two concept maps about roman empire")
	{
		string map1 = "The Roman Empire\tincluded\tLarge territorial holdings around the Mediterranean Sea\
					   Large territorial holdings around the Mediterranean Sea\truled by\tEmperors\
					   The Roman Empire\twas distinguished\tAlso for its accomplishments in intellectual endeavours";
		string map2 = "The Roman Empire\tincluded\twas\the most extensive political and social structure in western civilization\
					   Emperors\twere the rulers of\tThe Roman Empire\
					   The Roman Empire\twas distinguished\tAlso for its accomplishments in intellectual endeavours";
					   
		stringstream mapstream1(map1);
		stringstream mapstream2(map2);
		
		concept_map cm1 (mapstream1);
		concept_map cm2 (mapstream2);
		
		WHEN("The maps are compared")
		{
			THEN("The similarity is correct")
			{
				REQUIRE_THAT(cm1.similarity(cm2), WithinAbsMatcher(0.75f, 0.1f));
			}
		}
	}
}
