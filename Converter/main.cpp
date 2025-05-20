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

    std::string connectionString = "host=localhost port=5432 dbname=converter user=postgres password =12345Kikki";

    try
    {
        pqxx::connection connectionObject(connectionString.c_str());
        pqxx::work worker(connectionObject);
        pqxx::result response = worker.exec("SELECT * FROM rates WHERE date = '2025-02-22'");

        for (const auto& row : response) {
            std::cout << "Id: " << row[0].as<int>()
                << " Date: " << row[1].as<std::string>()
                << " Currency: " << row[2].as<std::string>()
                << " Rate: " << row[3].as<double>()
                << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    system("pause");

	//https_get();

	return 0;
}