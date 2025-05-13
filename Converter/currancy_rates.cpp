#include "currency_rates.h"

currencyRates::currencyRates() {
	rates["USD"] = 1.0;
	rates["EUR"] = 0.85;
	rates["GBP"] = 0.75;
	rates["JPY"] = 110.0;
	rates["UAH"] = 41.65;

}

double currencyRates::get_rate(const std::string& currency) const{
	auto it = rates.find(currency);
	if (it == rates.end()) {
		std::cout << "Currency not found" << std::endl;
		return -1.0;
	}
		
	return it->second;
}

void currencyRates::add_rate(const std::string& currency, double rate) {
	rates[currency] = rate;
}