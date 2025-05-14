#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

class json_parser {
private:
	std::string file_name;
	std::ifstream file;

public:
	json_parser();
	json_parser(const std::string& file_name);


	std::string get_file() const;
	void set_file_name(const std::string& file_name);
	void read();
	/*void read_str() ;*/
};

#endif // !JSON_PARSER_H

