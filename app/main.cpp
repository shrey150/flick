// Executables must have the following defined if the library contains
// doctest definitions. For builds with this disabled, e.g. code shipped to
// users, this can be left out.
#ifdef ENABLE_DOCTEST_IN_LIBRARY
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"
#endif

#include <iostream>

#include "flickConfig.h"
#include "ai.h"

int main(int argc, char* argv[])
{
	// print version info
	std::cout << "Flick v"
		<< PROJECT_VERSION_MAJOR
		<< "."
		<< PROJECT_VERSION_MINOR
		<< "."
		<< PROJECT_VERSION_PATCH
		<< std::endl << std::endl;

	// increment argv to skip first element (executable name);
	// "length" must also be 1 smaller to exclude executable name
	json j = gen_edits(++argv, argc-1);

	std::cout << "Suggested edits: " << j << std::endl;
}
