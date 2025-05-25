#ifndef API_REQUEST_HPP
#define API_REQUEST_HPP

#include <fstream>
#include <string>
#include <chrono>
#include <format>

// Handles API URL generation for currency rate requests
class apiRequest {
private:
	std::string access_key_ = "";

public:
	apiRequest();

	// Generates URL for current date rates
	std::string current_date_request() const;

	// Generates URL for historical rates on specified date
	std::string custom_date_request(const std::string& date) const;

	// Returns current date in YYYY-MM-DD format
	std::string get_current_date() const;

	bool isDateValid(const std::string& date) const;
};

#endif // !API_REQUEST_HPP
