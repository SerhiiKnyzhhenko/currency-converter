#include "json_parser.h"

json_parser::json_parser() : file("") {}

json_parser::json_parser(const std::string& file_name) : file(file_name) {}

std::string json_parser::get_file() const { return file; }

void json_parser::set_file(const std::string& file_name) {
	file = file_name;
}

void json_parser::open() const {

}

void json_parser::read() const {

}

void json_parser::close() const {

}