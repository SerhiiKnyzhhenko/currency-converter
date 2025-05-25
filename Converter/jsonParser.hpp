#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <nlohmann/json.hpp>
#include <unordered_map>
#include <string>
#include <fstream>
#include <stdexcept>
#include "dataBase.hpp"

// Class for parsing JSON data and interacting with a hash map or database
class jsonParser {
private:
	std::string file_name_ = "C:/Users/12345/OneDrive/Desktop/file.json";

public:
	// Populates a hash map with currency rates from the JSON file
	void write_to_hash(std::unordered_map<std::string, double>&);

	// Writes currency rates from the JSON file to the database
	void write_to_db(dataBase&);

private:
	// Parses the JSON file and returns the JSON object
	nlohmann::json parse_json() const;

	// Checks if the JSON response indicates a successful API call
	bool check_success(const nlohmann::json&) const;
};

#endif // !JSON_PARSER_H

