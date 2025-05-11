#ifndef CONVERTER_H
#define CONVERTER_H

#include <memory> 

class converter {
private:
	std::unique_ptr<token> token_from;
	std::unique_ptr<token> token_to;
	const currencyRates* rates;

public:
	converter(const currencyRates* rates);
	void set_tokens(const std::string& currency_from, 
					const std::string& currency_to, 
					double value);
	double convert() const;
};

#endif // !CONVERTER_H
