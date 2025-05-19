#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>

class json_parser {
private:
	std::string file_name;
	std::ifstream file;

public:
	json_parser();
	json_parser(const std::string&);

	double convert_to(const std::string& str);
	std::string get_file() const;
	void set_file_name(const std::string&);
	void read_and_add_rates(std::unordered_map<std::string, double>&);
};

#endif // !JSON_PARSER_H

