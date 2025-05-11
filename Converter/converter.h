#ifndef CONVERTER_H
#define CONVERTER_H
#include <memory> 

class converter {
private:
	std::unique_ptr<token> token1;
	std::unique_ptr<token> token2;

public:
	converter();
	void convert();
};

#endif // !CONVERTER_H
