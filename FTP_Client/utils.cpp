#include "utils.h"

std::ostream& Utils::operator << (std::ostream& o, const Utils::Color& color)
{	
	if (o.rdbuf() == std::cout.rdbuf())				
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color.code);
	return o;
}
