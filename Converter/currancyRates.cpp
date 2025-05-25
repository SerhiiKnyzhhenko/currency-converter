#include "currencyRates.hpp"

// Initialize with USD as the base currency (rate = 1.0)
currencyRates::currencyRates() {
	rates_["USD"] = 1.0;
}

// Provides direct access to the rates map (CAUTION: exposes internal data)
std::unordered_map<std::string, double>& currencyRates::get_rates() { 
	std::lock_guard<std::mutex> lock(rates_mutex_);
	return rates_; 
}

// Returns rate for a currency or -1.0 if not found (with error message)
double currencyRates::get_rate(const std::string& currency) const{
	std::lock_guard<std::mutex> lock(rates_mutex_);
	auto it = rates_.find(currency);
	if (it == rates_.end()) {
		throw std::runtime_error("Currency not found: " + currency);
	}	
	return it->second;
}

// Adds or updates a currency rate (no validation)
void currencyRates::add_rate(const std::string& currency, double rate) {
	std::lock_guard<std::mutex> lock(rates_mutex_);
	if (currency.size() != 3) 
		throw std::invalid_argument("Invalid currency code");
	if (rate <= 0) 
		throw std::invalid_argument("Rate must be positive");
	rates_[currency] = rate;
}