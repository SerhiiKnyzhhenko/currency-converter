#include <iostream>
#include "HttpServer.hpp"


int main() {
	try {
		HttpServer server(11000);
		server.start();
	}
	catch (const std::system_error& e) {
		std::cerr << "System error: " << e.what() << std::endl;
		return 1;
	}
	catch (const std::exception& e) {
		std::cerr << "Fatal error: " << e.what() << std::endl;
		return 1;
	}
	catch (...) {
		std::cerr << "Unknown error" << std::endl;
		return 1;
	}
	return 0;
}