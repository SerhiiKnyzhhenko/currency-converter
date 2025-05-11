#include "classes.h"
#include <string>


token::token() : curr(""), value(0.0) {}

token::token(std::string c, double v) : curr(c), value(v) {}

void token::set_currency(std::string c) { curr = c; }

void token::set_value(double v) { value = v; }

std::string token::get_currency() { return curr; }

double token::get_value() { return value; }

