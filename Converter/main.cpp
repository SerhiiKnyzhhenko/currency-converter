#include <iostream>
#include <memory>
#include "classes.h"
#include <vector>

int main() {
	
	auto rates_ptr = std::make_unique<currencyRates>();

	auto converter_ptr = std::make_unique<converter>(rates_ptr.get());
	
	std::string curr_from = "UAH";
	std::string curr_to = "USD";
	double val = 500;

	converter_ptr->set_tokens(curr_from, curr_to, val);

	std::cout << converter_ptr->convert() << std::endl;

	return 0;
}