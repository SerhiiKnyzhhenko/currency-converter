#include "json_parser.h"
#include <fstream>

json_parser::json_parser() : file_name(""), file() {}

json_parser::json_parser(const std::string& f_name) : file_name(f_name), file(f_name) {}

std::string json_parser::get_file() const { return file_name; }

void json_parser::set_file(const std::string& f_name) {
	file_name = f_name;
}

void json_parser::open() const {

}

void json_parser::read() const {

}

void json_parser::close() const {

}