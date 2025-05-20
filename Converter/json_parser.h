#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include "data_base.h"

class json_parser {
private:
	std::string file_name;
	std::ifstream file;

public:
	json_parser();
	json_parser(const std::string&);

	double convert_to_double(const std::string&);
	int convert_to_int(const std::string&);
	std::string get_file() const;
	void set_file_name(const std::string&);
	void read_and_add_to_hash(std::unordered_map<std::string, double>&);
	void read_adn_add_to_db(std::string);
};

#endif // !JSON_PARSER_H

