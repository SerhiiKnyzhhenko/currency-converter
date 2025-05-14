#include "json_parser.h"

json_parser::json_parser() : file_name(""), file() {}

json_parser::json_parser(const std::string& f_name) : file_name(f_name), file(f_name) {
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + file_name);
	}
}

std::string json_parser::get_file() const { return file_name; }

void json_parser::set_file_name(const std::string& f_name) {
	file_name = f_name;
	file.open("f_name");
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + file_name);
	}
}

void json_parser::read(std::unordered_map<std::string, double>& map) {
	if (!file.is_open())
		std::cout << "File is not open";
	else {
		std::string line;
		while (std::getline(file, line)) {
			size_t pos = 0;
			while (line.find("USD", pos) != std::string::npos) {
				size_t key_start = pos + 1;
				pos = line.find("USD", key_start) + 3;
				size_t key_end = line.find('"', pos);
				std::string key = line.substr(key_start + 7, key_end - pos);
				std::cout << key << std::endl;
			}
		}		
	}


}

