#include <iostream>
#include <memory>
#include "classes.hpp"
#include "httpClient.hpp"
#include <vector>
#include <string>

std::string file_name = "C:/Users/12345/OneDrive/Desktop/file.json";

int main() {
	
	auto rates_ptr = std::make_unique<currencyRates>();

	
	// its work!!!!
	/*httpClient* client = new httpClient("api.currencylayer.com");
	apiRequest* request = new apiRequest();
	std::string body = client->get_json_body(request->custom_date_request("2025-01-13"));
	responseHandler* rp = new responseHandler();

	rp->save_to_file(body, file_name);*/

	dataBase* db = new dataBase();

	/*jsonParser* jpars = new jsonParser(file_name);
	jpars->write_to_db(*db);*/

	db->add_resp_to_hash("2025-01-13", rates_ptr->get_rates());

	for (auto a : rates_ptr.get()->get_rates())
		std::cout << a.first << " " << a.second << std::endl;

    system("pause");

	//https_get();

	return 0;
}