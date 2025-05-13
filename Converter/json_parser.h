#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <iostream>
#include <fstream>
#include <string>

class json_parser {
private:
	std::string file_name;
	std::ifstream file;

public:
	json_parser();
	json_parser(const std::string& file_name);
	~json_parser();

	std::string get_file() const;
	void set_file(const std::string& file_name);
	void open() const;
	void read() const;
	void close() const;
};

#endif // !JSON_PARSER_H

