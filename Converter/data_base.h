#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <pqxx/pqxx>
#include <iostream>
#include <chrono>
#include <format>
#include <regex>

std::string conn = "host=localhost port=5432 dbname=converter user=postgres password =12345Kikki";

class data_base {
private:
	pqxx::connection connectionObject;
	pqxx::work worker;

public:
	data_base();
	/*data_base(const std::string& date);*/
	pqxx::result add_resp_to_hash(const std::string&, std::unordered_map<std::string, double>&);
	void add_to_db_from_hash(const std::string&, const std::unordered_map<std::string, double>&);
	void add_to_db_row(const std::string&, const std::string&, double);
	std::string get_current_date();
	std::string get_custom_date(int, int ,int);
	bool isDateValid(const std::string&);
};

#endif // !DATA_BASE_H

