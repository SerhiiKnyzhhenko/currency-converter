#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <pqxx/pqxx>

std::string conn = "host=localhost port=5432 dbname=converter user=postgres password =12345Kikki";

class data_base {
private:
	pqxx::connection connectionObject;
	pqxx::work worker;

	data_base();
	/*data_base(const std::string& date);*/
	pqxx::result new_response(const std::string&);
	void add_to_db(const std::string&, double);
	
};

#endif // !DATA_BASE_H

