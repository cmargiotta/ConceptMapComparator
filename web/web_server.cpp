#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#include <httplib.h>

#include <concept_map.hpp>

using namespace httplib;
using namespace std;

int main() {
	// Reading index.html
	ifstream t("web/index.html");
	stringstream buffer;
	buffer << t.rdbuf();
	string body = buffer.str();
		
	Server svr;

	svr.Get("/", [&](const Request&, Response& res) {
		res.set_content(body, "text/html");
	});
	
	svr.Post("/upload", [&](const auto& req, auto& res) {
		const auto& map1 = req.get_file_value("map1");
		const auto& map2 = req.get_file_value("map2");
		// file.filename;
		// file.content_type;
		// file.content;
		cout << "Maps received\n";
		
		try {
			stringstream ss1 = stringstream(map1.content);
			stringstream ss2 = stringstream(map2.content);
			concept_map cm1 (ss1);
			concept_map cm2 (ss2);
			
			cout << "Starting comparison\n";
			
			res.set_content(to_string(cm1.similarity(cm2)), "text/plain");
		} catch(std::runtime_error e)
		{
			res.set_content(e.what(), "text/plain");
		}
	});

	svr.listen("localhost", 8080);
}
