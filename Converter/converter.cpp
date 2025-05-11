#include "classes.h"
#include <memory>
#include <unordered_map>


converter::converter(const currencyRates* rates) :
	rates(rates),
	token_from(std::make_unique<token>()),
	token_to(std::make_unique<token>()) {}

void converter::set_tokens(const std::string& currency_from, 
						   const std::string& currency_to, 
						   double value_from) {
	token_from->set_currency(currency_from);
	token_to->set_currency(currency_to);
	token_from->set_value(value_from);
}

double converter::convert() const {
	const std::string from = token_from->get_currency();
	const std::string to = token_to->get_currency();
	const double from_value = token_from->get_value();

	const double rate_from = rates->get_rate(from);
	const double rate_to = rates->get_rate(to);

	if (rate_from < 0 || rate_to < 0)
		std::cout << "Unknown currency" << std::endl;

	return from_value * (rate_to / rate_from);
}
