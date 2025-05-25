#include "jsonParser.hpp"

using json = nlohmann::json;

// Extracts currency rates from JSON and stores them in the provided hash map
void jsonParser::write_to_hash(std::unordered_map<std::string, double>& rates) {
	try {
		json data = parse_json();
		if (!check_success(data)) return; //Exit if API call failed

		json& quotes = data["quotes"];
		if (!quotes.is_object()) {
			throw std::runtime_error("'quotes' is not an object");
		}

		// Process each USD-prefixed currency pair
		for (auto& [key, value] : data["quotes"].items()) {
			if (key.size() >= 3 && key.substr(0, 3) == "USD") {
				std::string currency = key.substr(3); // Extract currency code (e.g., "AED")
				rates[currency] = value.get<double>();
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "JSON Error: " << e.what() << std::endl;
	}
}

// Inserts parsed currency rates into the database
void jsonParser::write_to_db(dataBase& db) {
	try {
		json data = parse_json();
		if (!check_success(data)) return;

		json& quotes = data["quotes"];
		if (!quotes.is_object()) {
			throw std::runtime_error("'quotes' is not an object");
		}

		// ---------------------------------------------------------------------
		// DATE VALIDATION BLOCK
		// ---------------------------------------------------------------------
		// 1. Check if "date" field exists and is not null
		if (!data.contains("date") || data["date"].is_null()) {
			throw std::runtime_error("Missing 'date' field in JSON");
		}

		// 2. Extract date value as string
		std::string date = data["date"].get<std::string>();

		// 3. Ensure date is not empty
		if (date.empty()) {
			throw std::runtime_error("'date' field is empty");
		}

		// 4. Validate date format using database utility method
		if (!db.isDateValid(date)) {
			throw std::runtime_error("Invalid date format: " + date);
		}
		// ---------------------------------------------------------------------
		
		// Insert each valid currency rate into the database
		for (auto& [key, value] : data["quotes"].items())
			if (key.size() >= 3 && key.substr(0, 3) == "USD") {
				std::string currency = key.substr(3); // "AED", "AFN" and else
				db.add_to_db_row(date, currency, value.get<double>());
			}			
		}
	catch (const std::exception& e) {
		std::cerr << "DB Error: " << e.what() << std::endl;
		throw;
	}
}

// Parses the JSON file and returns the parsed data
json jsonParser::parse_json() const {
	std::ifstream file(file_name_);
	if (!file.is_open()) throw std::runtime_error("File not open");
	return json::parse(file); // Parse JSON content
}

// Checks if the JSON response contains a "success" flag set to true
bool jsonParser::check_success(const json& data) const {
	return data.value("success", false); // Default to false if "success" is missing
}




