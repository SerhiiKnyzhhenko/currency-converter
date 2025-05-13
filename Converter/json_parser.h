#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>

class json_parser {
private:
	std::string file;

public:
	json_parser();
	json_parser(const std::string& file_name);
	~json_parser();

	std::string get_file();
	void set_file(const std::string& file_name);
	void open();
	void close();
};

#endif // !JSON_PARSER_H

