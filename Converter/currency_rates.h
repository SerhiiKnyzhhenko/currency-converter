#ifndef CURRENCY_RATES_H
#define CURRENCY_RATES_H

#include <unordered_map>
#include <string>

class currencyRates {
private:
	std::unordered_map<std::string, double> rates;
	const std::string base_currecy = "USD";

public:
	currencyRates();
	double get_rate(const std::string& currency) const;
	void add_rate(const std::string& currency, double rate);

};

#endif // !CURRANCY_RATES_H

