#include "jsonParser.hpp"

using json = nlohmann::json;

jsonParser::jsonParser(const std::string& f_name) : file_name_(f_name) {}

void jsonParser::write_to_hash(std::unordered_map<std::string, double>& rates) {
	try {
		json data = parse_json();
		if (!check_success(data)) return;

		json& quotes = data["quotes"];
		if (!quotes.is_object()) {
			throw std::runtime_error("'quotes' is not an object");
		}

		for (auto& [key, value] : data["quotes"].items()) {
			if (key.size() >= 3 && key.substr(0, 3) == "USD") {
				std::string currency = key.substr(3); // "AED", "AFN" and else
				rates[currency] = value.get<double>();

				std::cout << currency << " " << value.get<double>() << std::endl;
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "JSON Error: " << e.what() << std::endl;
	}
}

void jsonParser::write_to_db(data_base& db) {
	try {
		json data = parse_json();
		if (!check_success(data)) return;

		json& quotes = data["quotes"];
		if (!quotes.is_object()) {
			throw std::runtime_error("'quotes' is not an object");
		}

		std::string date = data.value("date", "");
		for (auto& [key, value] : data["quotes"].items())
			if (key.size() >= 3 && key.substr(0, 3) == "USD") {
				std::string currency = key.substr(3); // "AED", "AFN" and else
				db.add_to_db_row(date, key, value.get<double>());
			}			
		}
	catch (const std::exception& e) {
		std::cerr << "DB Error: " << e.what() << std::endl;
	}

}

nlohmann::json jsonParser::parse_json() const {
	std::ifstream file(file_name_);
	if (!file.is_open()) throw std::runtime_error("File not open");
	return json::parse(file);
}

bool jsonParser::check_success(const nlohmann::json& data) const {
	return data.value("success", false);
}




