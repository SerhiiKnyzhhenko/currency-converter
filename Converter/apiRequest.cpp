#include "apiRequest.hpp"
#include <format>

std::string apiRequest::current_date_request() const {
	return std::format("/live?access_key={}&format=1", access_key_);
}

std::string apiRequest::custom_date_request(const std::string& date) const {
	return std::format("/historical?access_key={}&date={}&format=1", access_key_, date);
}

std::string apiRequest::get_current_date() {
	auto now = std::chrono::system_clock::now();
	auto today = std::chrono::current_zone()->to_local(now);
	auto ymd = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(today));

	return std::format("{:%Y-%m-%d}", ymd);
}