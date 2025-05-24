#ifndef API_REQUEST_HPP
#define API_REQUEST_HPP

#include <string>
#include <chrono>
#include <format>

class apiRequest {
private:
	std::string access_key_ = "50f2dab13e92481fb0ae68c221ba2716";

public:
	std::string current_date_request() const;
	std::string custom_date_request(const std::string&) const;

	std::string get_current_date();
};

#endif // !API_REQUEST_HPP
