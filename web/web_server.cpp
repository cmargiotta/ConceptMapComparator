#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#include <httplib.h>

using namespace httplib;
using namespace std;

int main() {
	// Reading index.html
	ifstream t("./index.html");
	stringstream buffer;
	buffer << t.rdbuf();
	string body = buffer.str();
		
	Server svr;

	svr.Get("/", [&](const Request&, Response& res) {
		res.set_content(body, "text/html");
	});
	
	svr.Post("/upload", [&](const auto& req, auto& res) {
		auto size = req.files.size();

		const auto& map1 = req.get_file_value("map1");
		const auto& map2 = req.get_file_value("map2");
		// file.filename;
		// file.content_type;
		// file.content;
		
		res.set_content(map1.content, "text/plain");
	});

	svr.listen("localhost", 8080);
}
