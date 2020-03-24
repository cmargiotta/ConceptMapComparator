#include "word_to_singular.hpp"

#include <regex>
#include <vector>
#include <utility>

using std::string;
using std::regex;
using std::regex_match;
using std::regex_replace;
using std::vector;
using std::pair;

string get_singular(string word)
{
	vector<pair<regex, string>> expressions = {
		{regex("equipment"), 			"equipment"},
		{regex("information"), 			"information"},
		{regex("rice"), 				"rice"},
		{regex("money"), 				"money"},
		{regex("species"), 				"species"},
		{regex("series"), 				"series"},
		{regex("fish"), 				"fish"},
		{regex("sheep"), 				"sheep"},
		{regex("jeans"), 				"jeans"},
		{regex("police"), 				"police"},
		{regex("zombies"), 				"zombie"},
		{regex("moves"), 				"move"},
		{regex("sexes"),				"sex"},
		{regex("(buffal|tomat)oes"),	"$1o"},
		{regex("(.*)children"),			"$1child"},
		{regex("(.*)men"),				"$1man"},
		{regex("(.*)people"), 			"$1person"},
		{regex("databases"),			"database"},
		{regex("quizzes"),				"quiz"},
		{regex("matrices"),				"matrix"},
		{regex("(vert|ind)ices"),		"$1ex"},
		{regex("oxen"),					"ox"},
		{regex("(alias|status)es"),		"$1"},
		{regex("(octop|vir)(us|i)"),	"$1us"},
		{regex("ax[ie]s"),				"axis"},
		{regex("(cris|test)(is|es)"),	"$1is"},
		{regex("shoes"),				"shoe"},
		{regex("oes"),					"o"},
		{regex("buses"),				"bus"},
		{regex("(m|l)ice"),				"$1ouse"},
		{regex("(.*)(sis|ses)"),		"$1sis"},
		{regex("(.*(x|ch|ss|s))es"),	"$1"},
		{regex("movies"),				"movie"},
		{regex("series"),				"series"},
		{regex("(.*([^aeiouy]|qu]))ies"),"$1y"},
		{regex("(.*[lr])ves"),			"$1f"},
		{regex("(.*[ht])ives"),			"$1ive"},
		{regex("(.*)ves"),				"$1fe"},
		//{regex("((a)naly|(b)a|(d)iagno|(p)arenthe|(p)rogno|(s)ynop|(t)he)(sis|ses)$"), "$1sis"},
		{regex("(.*ri)a"),				"$1on"},
		{regex("(.*[ti])a"),			"$1um"},
		{regex("(.*)news"),				"$1news"},
		{regex("(.*)s"),				"$1"}
	};
	
	for (pair<regex, string>& expr: expressions)
	{
		if (regex_match(word, expr.first))
		{
			return regex_replace(word, expr.first, expr.second);
		}
	}
	
	return "";
}
