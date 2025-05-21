#ifndef API_REQUEST_HPP
#define API_REQUEST_HPP

#include <string>

class apiRequest {
private:
	std::string access_key_ = "50f2dab13e92481fb0ae68c221ba2716";

public:
	std::string current_date_request() const;
	std::string custom_date_request(const std::string&) const;
};

#endif // !API_REQUEST_HPP
