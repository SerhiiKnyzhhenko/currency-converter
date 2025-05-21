#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class token
{
private:
	std::string curr;
	double value;

public:
	token();
	token(std::string c, double v);

	void set_currency(std::string c);
	void set_value(double v);
	std::string get_currency();
	double get_value();
};

#endif // !TOKEN_H
