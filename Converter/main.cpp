#include <iostream>
#include <memory>
#include "classes.hpp"
#include "httpClient.hpp"
#include "HttpServer.hpp"
#include "HttpServerBoost.hpp"
#include <filesystem>

namespace fs = std::filesystem;

std::string file_name = "C:/Users/12345/OneDrive/Desktop/file.json";

int main() {

	try {
		HttpServer server(11000); // Создание сервера
		server.start();
	}
	catch (const std::exception& e) {
		std::cerr << "Fatal error: " << e.what() << std::endl;
		return 1;
	}


	//----------------------------------------------BOOST HttpServ
	/*try {
		asio::io_context io_context;
		HttpServerBoost server(io_context, 11000);
		server.start();
		io_context.run();
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	return 0;*/
	//----------------------------------------------BOOST HttpServ
	// 
	//https_get();

	return 0;
}