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

void json_parser::read_and_add_to_hash(std::unordered_map<std::string, double>& rates) {
	if (!file.is_open())
		std::cout << "File is not open";
	else {

		std::string line;

		while (std::getline(file, line)) {

			size_t pos = 0;
			double rate = 0.0;

			while (line.find("USD", pos) != std::string::npos) {

				size_t key_start = pos + 1;
				pos = line.find("USD", key_start) + 3;
				size_t key_end = line.find('"', pos);
				std::string key = line.substr(key_start + 7, key_end - pos);
				
				if (key != "") {
					key_start = line.find(':', pos) + 1;
					key_end = line.find(',', key_start);
					std::string str = line.substr(key_start, key_end - key_start);
					rate = convert_to_double(str);
				}
				if (key != "" && rate > 0)
					rates[key] = rate;
			}
		}		
	}
}

void json_parser::read_adn_add_to_db(std::string date = "") {
	if (!file.is_open())
		std::cout << "File is not open";
	else {

		data_base db;
		std::string line;

		while (std::getline(file, line)) {

			double rate = 0.0;
			size_t pos = 0;

			if (line.find("\"success\":", pos) != std::string::npos) {
				pos = line.find("\"success\":", pos);
				size_t key_start = pos + 10;
				size_t key_end = line.find(",", key_start);
				std::string result = line.substr(key_start, key_end - key_start);
				if (result == "false")
					return;
			}

			if (line.find("\"date\":\"", pos) != std::string::npos) {
				pos = line.find("\"date\":\"", pos);
				date = line.substr(10, 10);	
			}

			while (line.find("USD", pos) != std::string::npos) {
				
				size_t key_start = pos + 1;
				pos = line.find("USD", key_start) + 3;
				size_t key_end = line.find('"', pos);
				std::string key = line.substr(key_start + 7, key_end - pos);

				if (key != "") {
					key_start = line.find(':', pos) + 1;
					key_end = line.find(',', key_start);
					std::string str = line.substr(key_start, key_end - key_start);
					rate = convert_to_double(str);
				}
				if (key != "" && rate > 0)
					db.add_to_db_row(date, key, rate);
			}
		}
	}

}

//std::string json_parser::get_date(const std::string& date) {
//	size_t pos = date.find("\"date\":\"", pos);
//	size_t key_start_year = pos + 8;
//	size_t key_end_year = key_start_year + 4;
//	size_t key_start_mounth = key_end_year + 1;
//	size_t key_end_mounth = key_end_mounth + 2;
//	size_t key_start_day = key_end_mounth + 1;
//	size_t key_end_day = key_start_day + 2;
//
//	int year = convert_to_int(date.substr(key_start_year, key_end_year - key_start_year));
//	int mounth = convert_to_int(date.substr(key_start_mounth, key_end_mounth - key_start_mounth));
//	int day = convert_to_int(date.substr(key_start_day, key_end_day - key_start_day));
//}



double json_parser::convert_to_double(const std::string& str) {
	std::istringstream ss(str);
	long double num;
	ss >> num;
	return num;
}

int json_parser::convert_to_int(const std::string& str) {
	std::istringstream ss(str);
	int num;
	ss >> num;
	return num;
}

