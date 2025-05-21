#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <nlohmann/json.hpp>
#include <unordered_map>
#include <string>
#include <fstream>
#include <stdexcept>
#include "dataBase.hpp"



class jsonParser {
private:
	std::string file_name_;

public:
	explicit jsonParser(const std::string&);

	void write_to_hash(std::unordered_map<std::string, double>&);
	void write_to_db(dataBase&);

private:
	nlohmann::json parse_json() const;
	bool check_success(const nlohmann::json&) const;
};

#endif // !JSON_PARSER_H

