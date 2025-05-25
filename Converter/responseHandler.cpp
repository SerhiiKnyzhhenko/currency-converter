#include "responeHandler.hpp"
#include <fstream>

void responseHandler::save_to_file(
	const std::string& res,
	const std::string& fPath) {
	std::ofstream file(fPath);
	if (!file) throw std::runtime_error("Failed to open file: " + fPath);
	file << res;
}

void responseHandler::save_to_file(const std::string& res) {
	const std::string& filename = "C:/Users/12345/OneDrive/Desktop/file.json";
	std::ofstream file(filename);
	if (!file) throw std::runtime_error("Failed to open file: " + filename);
	file << res;
}