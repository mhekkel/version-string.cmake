#include "revision.hpp"
#include "mylib.hpp"

#include <iostream>

int main()
{
	(void)foo();

	std::cout << "verbose false:\n";
	write_version_string(std::cout, false);
	std::cout << '\n';

	std::cout << "verbose true:\n";
	write_version_string(std::cout, true);
	std::cout << '\n';

	return 0;
}