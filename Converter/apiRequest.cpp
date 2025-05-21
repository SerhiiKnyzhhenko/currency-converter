#include "apiRequest.hpp"
#include <format>

std::string apiRequest::current_date_request() const {
	return std::format("/live?access_key={}&format=1", access_key_);
}

std::string apiRequest::custom_date_request(const std::string& date) const {
	return std::format("/historical?access_key={}&date={}&format=1", access_key_, date);
}