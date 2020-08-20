#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#include "lithium_http_backend.hh"

using namespace li;
using namespace std;

int main() {
	// Reading index.html
	ifstream t("./index.html");
	stringstream buffer;
	buffer << t.rdbuf();
	string body = buffer.str();
	
	cout << body;
	
	http_api api;

	api.get("") = [&](http_request&, http_response& response) {
		cout << body;
		response.write(body);
	};

	http_serve(api, 8080);
}
