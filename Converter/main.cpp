#include <iostream>
#include <memory>
#include "classes.h"
#include <vector>

int main() {
	
	auto rates_ptr{ std::make_unique<currencyRates>() };

	auto converter_ptr{ std::make_unique<converter>(rates_ptr) };
	

	return 0;
}