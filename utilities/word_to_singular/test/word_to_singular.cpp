#include <catch.hpp>

#include <string>
#include <vector>

#include <word_to_singular.hpp>

using std::string;
using std::vector;

using Catch::Matchers::Equals; 

SCENARIO("Plural words to singular")
{
	GIVEN("A set of plural words")
	{
		vector<string> plurals = {
			"dogs",
			"pugs",
			"stars",
			"buses",
			"fish",
			"mice",
			"queries",
			"abilities",
			"agencies",
			"movies",
			"archives",
			"indices",
			"wives",
			"saves",
			"halves",
			"moves",
			"salespeople",
			"people",
			"spokesmen",
			"men",
			"women",
			"bases",
			"diagnoses",
			"data",
			"media",
			"stadia",
			"analyses",
			"node_children",
			"children",
			"experiences",
			"days",
			"comments",
			"foobars",
			"newsletters",
			"old_news",
			"news",
			"series",
			"species",
			"quizzes",
			"perspectives",
			"oxen",
			"photos",
			"buffaloes",
			"tomatoes",
			"dwarves",
			"elves",
			"information",
			"equipment",
			"criteria"
		};
		
		vector<string> singulars = {
			"dog",
			"pug",
			"star",
			"bus",
			"fish",
			"mouse",
			"query",
			"ability",
			"agency",
			"movie",
			"archive",
			"index",
			"wife",
			"safe",
			"half",
			"move",
			"salesperson",
			"person",
			"spokesman",
			"man",
			"woman",
			"basis",
			"diagnosis",
			"datum",
			"medium",
			"stadium",
			"analysis",
			"node_child",
			"child",
			"experience",
			"day",
			"comment",
			"foobar",
			"newsletter",
			"old_news",
			"news",
			"series",
			"species",
			"quiz",
			"perspective",
			"ox",
			"photo",
			"buffalo",
			"tomato",
			"dwarf",
			"elf",
			"information",
			"equipment",
			"criterion"
		};
		
		WHEN("Their singular is computed")
		{
			vector<string> computed_singulars;
			
			for (auto p: plurals)
			{
				computed_singulars.push_back(get_singular(p));
			}
			
			THEN("It is correct")
			{
				REQUIRE_THAT(singulars, Equals(computed_singulars));
			}
		}
	}
	
	
}
