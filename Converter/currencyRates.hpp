#ifndef CURRENCY_RATES_H
#define CURRENCY_RATES_H

#include <unordered_map>
#include <string>
#include <optional>
#include <mutex>
#include <iostream>

// Class for storing and managing currency exchange rates
class currencyRates {
private:
	std::unordered_map<std::string, double> rates_; // Key: currency code, Value: rate	
	mutable std::mutex rates_mutex_;

public:
	currencyRates(); // Initializes default rates (USD = 1.0)	
	std::unordered_map<std::string, double>& get_rates(); // Returns reference to all rates	
	double get_rate(const std::string& currency) const; // Returns rate or -1.0 if not found
	void add_rate(const std::string& currency, double rate); // Adds/updates a rate

};

#endif // !CURRENCY_RATES_H

