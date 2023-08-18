#include "revision.hpp"
#include "mylib.hpp"

#include <iostream>

int main()
{
	(void)foo();

	std::cout << "verbose false:" << std::endl;
	write_version_string(std::cout, false);
	std::cout << std::endl;

	std::cout << "verbose true:" << std::endl;
	write_version_string(std::cout, true);
	std::cout << std::endl;

	return 0;
}