#ifndef RESPONSE_HANDLER_HPP
#define RESPONSE_HANDLER_HPP

#include <boost/beast/http.hpp>
#include <string>

class responseHandler {
public:
	static void save_to_file(const std::string& res, const std::string& fPath);
	static void save_to_file(const std::string& res);
};

#endif // !RESPONSE_HANDLER_HPP
