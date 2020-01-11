//#include <catch.hpp>

#include <wordnet.hpp>
#include <iostream>

using namespace std;
//using namespace Catch::Matchers;

int main()
{
	wordnet wn("wordnet/dict/");

	auto dog = wn.words["dog"];
	
	cout << dog[0] << endl;
	
	cout << wn.hypernyms[dog[0]] << endl;
}


