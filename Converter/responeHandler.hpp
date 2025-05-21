#ifndef RESPONSE_HANDLER_HPP
#define RESPONSE_HANDLER_HPP

#include <boost/beast/http.hpp>
#include <string>

class responseHandler {
public:
	static void save_to_file(
		const std::string&,
		const std::string&);
};

#endif // !RESPONSE_HANDLER_HPP
