#include <iostream>
#include <memory>
#include "classes.h"
#include "https.h"
#include <vector>
#include <string>


int main() {

	//auto rates_ptr = std::make_unique<currencyRates>();

	/*auto converter_ptr = std::make_unique<converter>(rates_ptr.get());
	
	std::string curr_from = "UAH";
	std::string curr_to = "USD";
	double val = 500;

	converter_ptr->set_tokens(curr_from, curr_to, val);

	std::cout << converter_ptr->convert() << std::endl;*/

	//json_parser j_pars("C:/users/12345/onedrive/desktop/file.json");
	//j_pars.read_and_add_rates(rates_ptr.get()->get_rates());

	//std::cout << rates_ptr.get()->get_rate("BTC");
    
	data_base db;

	std::string date = db.get_current_date();

	get_json_for_custom_date(date);

    system("pause");

	//https_get();

	return 0;
}