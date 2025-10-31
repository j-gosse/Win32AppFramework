/*!
win32\TestSuite\TestSuite.hpp
Created: October 21, 2025
Updated: October 26, 2025
Copyright (c) 2025, Jacob Gosse

Test Suite header file.
*/

#pragma once

#ifndef TESTSUITE_HPP_
#define TESTSUITE_HPP_

#include <win32/framework.h>

class TestRegistry
{
public:
	class TestCase
	{
	private:
		std::string caseName_;
		std::string groupName_;
		std::string sectionName_;
		std::uintmax_t testsChecked_ = 0;
		std::uintmax_t testsPassed_ = 0;
		double caseWeight_ = 1.0;
		static const std::string defaultGroup_;
		static const std::string defaultSection_;
		std::chrono::nanoseconds elapsed_{};

	public:
		friend class TestRegistry;
		TestCase(TestCase const&) = delete;
		void operator = (TestCase const&) = delete;

		TestCase(std::string const& name, std::string const& group, std::string const& section, double weight = 1.0);
		TestCase(std::string const& name) : TestCase(name, defaultGroup_, defaultSection_) {}
		TestCase(std::string const& name, double weight) : TestCase(name, defaultGroup_, defaultSection_, weight) {}
		TestCase(std::string const& name, std::string const& group) : TestCase(name, group, defaultSection_) {}
		TestCase(std::string const& name, std::string const& group, double weight) : TestCase(name, group, defaultSection_, weight) {}

		virtual ~TestCase() = default;

		void Check(bool condition, const char* const conditionString, const char* const file, int line);
		void CheckMsg(bool condition, const std::string& message, const char* const file, int line);
		template <typename LHS, typename RHS>
		void CheckEqual(const LHS& lhs, const RHS& rhs, const char* lhsString, const char* rhsString, const char* const file, int line);
		template <typename LHS, typename RHS, typename VALUE>
		void CheckWithin(const LHS& lhs, const RHS& rhs, const VALUE& min, const char* lhsString, const char* rhsString, const char* minString, const char* const file, int line);
		virtual void Run() = 0;

		std::string CaseName() const { return caseName_; }
		std::string GroupName() const { return groupName_; }
		std::string SectionName() const { return sectionName_; }
		std::uintmax_t TestsChecked() const { return testsChecked_; }
		std::uintmax_t TestsPassed() const { return testsPassed_; }
		double CaseWeight() const { return caseWeight_; }
		std::chrono::nanoseconds Elapsed() const { return elapsed_; }

		constexpr auto operator <=> (TestCase const& rhs) const { return caseName_ <=> rhs.caseName_; }
		constexpr bool operator == (TestCase const& rhs) const { return caseName_ == rhs.caseName_; }
	};

private:
	static std::unique_ptr<std::map<std::string, std::map<std::string, std::vector<TestCase*>>>> casesPtr_;
	static TestCase* currentCasePtr_;
	static std::ofstream logFile_;

	static std::map<std::string, std::map<std::string, std::vector<TestCase*>>>& Cases();
	int Run();

public:
	TestRegistry();
	virtual ~TestRegistry();

	static TestCase* CurrentCase(const char* file, int line);
	static void RunAll();
};

template <typename LHS, typename RHS>
void TestRegistry::TestCase::CheckEqual(const LHS& lhs, const RHS& rhs, const char* lhsString, const char* rhsString, const char* const file, int line)
{
	bool condition = lhs == rhs;
	++testsChecked_;
	if (!condition)
	{
		std::basic_ostringstream<char> oss;
		std::basic_ostream<char>& os = oss;
		std::filesystem::path f = file;
		os << f.filename().string() << " (" << line << "): error in \"" << caseName_ << "\": ";
		oss << "\"" << lhsString << "\" [" << lhs << "] != \"" << rhsString << "\" [" << rhs << "]\n";
		std::cout << oss.str();
		if (logFile_.is_open()) logFile_ << oss.str();
	}
	else ++testsPassed_;
}

template <typename LHS, typename RHS, typename VALUE>
void TestRegistry::TestCase::CheckWithin(const LHS& lhs, const RHS& rhs, const VALUE& min, const char* lhsString, const char* rhsString, const char* minString, const char* const file, int line)
{
	bool condition = std::abs((lhs)-(rhs)) <= std::abs(min);
	++testsChecked_;
	if (!condition)
	{
		std::basic_ostringstream<char> oss;
		std::basic_ostream<char>& os = oss;
		std::filesystem::path f = file;
		os << f.filename().string() << " (" << line << "): error in \"" << caseName_ << "\": ";
		oss << "difference(" << lhsString << ", " << rhsString << ") > " << minString << " ==> \t|" << lhs << " - " << rhs << "| > " << std::abs(min) << '\n';
		std::cout << oss.str();
		if (logFile_.is_open()) logFile_ << oss.str();
	}
	else ++testsPassed_;
}

#endif