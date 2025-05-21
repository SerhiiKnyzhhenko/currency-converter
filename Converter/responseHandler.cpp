#include "responeHandler.hpp"
#include <fstream>

void responseHandler::save_to_file(
	const boost::beast::http::response<boost::beast::http::dynamic_body>& res,
	const std::string& filename
) {
	std::ofstream file(filename);
	if (!file) throw std::runtime_error("Failed to open file: " + filename);
	file << res;
}