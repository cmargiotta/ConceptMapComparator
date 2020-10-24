#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#include <httplib.h>

#include <concept_map.hpp>

using namespace httplib;
using namespace std;

int main() {
	wordnet::get_instance("../wordnet/dict/wordnet.db");
	
	// Reading index.html
	ifstream index ("web/index.html");
	ifstream js ("web/src.js");
	
	stringstream index_buffer, script_buffer;
	
	index_buffer << index.rdbuf();
	string body = index_buffer.str();
	
	script_buffer << js.rdbuf();
	string script = script_buffer.str();
		
	Server svr;

	svr.Get("/", [&](const Request&, Response& res) {
		res.set_content(body, "text/html");
	});
	
	svr.Get("/src.js", [&](const Request&, Response& res) {
		res.set_content(script, "application/javascript");
	});
	
	svr.Post("/upload", [&](const auto& req, auto& res) {
		const auto& map1 = req.get_file_value("map1");
		const auto& map2 = req.get_file_value("map2");
		
		try {
			stringstream ss1 = stringstream(map1.content);
			stringstream ss2 = stringstream(map2.content);
			concept_map cm1 (ss1);
			concept_map cm2 (ss2);
			
			res.set_content(to_string(cm1.similarity(cm2)), "text/plain");
		} catch(std::runtime_error& e)
		{
			res.set_content(string("ERROR:") + e.what(), "text/plain");
		}
	});

	svr.listen("localhost", 8080);
}
