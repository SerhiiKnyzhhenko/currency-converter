#include "data_base.h"

data_base::data_base() : connectionObject(conn), worker(connectionObject) {}

pqxx::result data_base::new_response(const std::string& response_text) {

}

void data_base::add_to_db(const std::string& currency, double rate) {

}