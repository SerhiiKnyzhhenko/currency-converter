#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <pqxx/pqxx>
#include <iostream>
#include <fstream>
#include <chrono>
#include <format>
#include <regex>
#include <memory>

extern std::string conn;

class dataBase {
private:
	std::unique_ptr<pqxx::connection> connectionObject_;
	std::unique_ptr<pqxx::work> worker_;

public:
	dataBase();
	
	void add_resp_to_hash(const std::string&, std::unordered_map<std::string, double>&);
	void add_to_db_from_hash(const std::string&, const std::unordered_map<std::string, double>&);
	void add_to_db_row(const std::string&, const std::string&, double);
	std::string get_current_date();
	std::string get_custom_date(int, int ,int);
	bool checkDataForDate(const std::string& date);

private:
	bool isDateValid(const std::string&);
};

#endif // !DATA_BASE_H

