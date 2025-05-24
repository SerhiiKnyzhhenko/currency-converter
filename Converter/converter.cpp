//#include "classes.hpp"
//#include <memory>
//#include <unordered_map>
//
//
//converter::converter() :
//	token_from(std::make_unique<token>()),
//	token_to(std::make_unique<token>()) {}
//
//void converter::set_tokens(const std::string& currency_from, 
//						   const std::string& currency_to, 
//						   double value_from) {
//	token_from->set_token(currency_from, value_from);
//	token_to->set_token(currency_to);
//}
//
//double converter::convert(double from, double to) const {
//	const double from_value = token_from->get_value();
//
//	if (from < 0 || to < 0)
//		std::cout << "Unknown currency" << std::endl;
//
//	return from_value * (to / from);
//}
