/*!
lib\source\TestSuite\tests\string_tests.cpp
Created: October 27, 2025
Updated: November 2, 2025
Copyright (c) 2025, Jacob Gosse

String Tests source file.
*/

#include <TestSuite/test_macros.hpp>
#include <utils/string_utils.hpp>

namespace winxframe
{
	TEST_CASE(ut_string_01, "String Tests")
	{
		static std::array<const std::string, 13> const names1 = {
			"",
			"January", "February", "March", "April", "May", "June", "July",
			"August", "September", "October", "November", "December"
		};
		static std::array<const std::string, 13> const names2 = {
			"",
			"January", "February", "March", "April", "May", "June", "July",
			"August", "September", "October", "November", "December"
		};

		for (int i = 1; i <= 12; ++i)
		{
			CHECK(names1[i] == names2[i]);
		}

		// check with std::string::compare
		for (int i = 1; i <= 12; ++i)
		{
			int result = names1[i].compare(names2[i]);
			CHECK(result == 0);
		}
	}
}; // end of namespace winxframe