#include "revision.hpp"

#include <iostream>

int main()
{
	std::cout << "verbose false:" << std::endl;
	write_version_string(std::cout, false);
	std::cout << std::endl;

	std::cout << "verbose true:" << std::endl;
	write_version_string(std::cout, true);
	std::cout << std::endl;

	return 0;
}