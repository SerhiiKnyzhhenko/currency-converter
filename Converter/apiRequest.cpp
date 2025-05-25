#include "apiRequest.hpp"


// Builds API endpoint URL for live exchange rates
std::string apiRequest::current_date_request() const {
	return std::format("/live?access_key={}&format=1", access_key_);
}

// Builds API endpoint URL for historical rates
std::string apiRequest::custom_date_request(const std::string& date) const {
    if (isDateValid(date))
        return std::format("/historical?access_key={}&date={}&format=1", access_key_, date);
    else
        throw;
}

// Returns current local date as string
std::string apiRequest::get_current_date() const{
	auto now = std::chrono::system_clock::now();
	auto today = std::chrono::current_zone()->to_local(now);
	auto ymd = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(today));

	return std::format("{:%Y-%m-%d}", ymd);
}

bool apiRequest::isDateValid(const std::string& date) const {
    try {
        std::chrono::year_month_day ymd{};
        std::istringstream iss(date);
        iss >> std::chrono::parse("%F", ymd);
        return ymd.ok();
    }
    catch (...) {
        return false;
    }
}