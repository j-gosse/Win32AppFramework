/*!
lib\include\TestSuite\TestSuite.hpp
Created: October 21, 2025
Updated: November 2, 2025
Copyright (c) 2025, Jacob Gosse

Test Suite header file.

\note
Testing structure and utilization is based on how testing is handled with Boost, Doctest, Catch2, and other similar unit testing frameworks.
*/

#pragma once

#ifndef TESTSUITE_HPP_
#define TESTSUITE_HPP_

#include <win32/framework.h>

namespace winxframe
{
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
			const std::string defaultGroupName_ = "default";
			const std::string defaultSectionName_ = "default";
			std::chrono::nanoseconds caseElapsed_{};

		public:
			friend class TestRegistry;
			TestCase(TestCase const&) = delete;
			void operator = (TestCase const&) = delete;

			TestCase(const std::string& name, const std::string& group, const std::string& section, double weight = 1.0);
			TestCase(const std::string& name) : TestCase(name, defaultGroupName_, defaultSectionName_) {}
			TestCase(const std::string& name, double weight) : TestCase(name, defaultGroupName_, defaultSectionName_, weight) {}
			TestCase(const std::string& name, const std::string& group) : TestCase(name, group, defaultSectionName_) {}
			TestCase(const std::string& name, const std::string& group, double weight) : TestCase(name, group, defaultSectionName_, weight) {}
			virtual ~TestCase() = default;

			void Check(bool condition, const char* const conditionString, const char* const file, int line);
			void Check(bool condition, const std::string& message, const char* const file, int line);
			template <typename LHS, typename RHS>
			void CheckEqual(const LHS& lhs, const RHS& rhs, const char* lhsString, const char* rhsString, const char* const file, int line);
			template <typename LHS, typename RHS, typename Value>
			void CheckWithin(const LHS& lhs, const RHS& rhs, const Value& min, const char* lhsString, const char* rhsString, const char* minString, const char* const file, int line);
			void LogCheckFail(const char* const conditionString, const char* const file, int line) const noexcept;
			void LogCheckFail(const std::string& message, const char* const file, int line) const noexcept;
			template <typename LHS, typename RHS>
			void LogCheckEqualFail(const LHS& lhs, const RHS& rhs, const char* lhsString, const char* rhsString, const char* const file, int line) const noexcept;
			template <typename LHS, typename RHS, typename Value>
			void LogCheckWithinFail(const LHS& lhs, const RHS& rhs, const Value& min, const char* lhsString, const char* rhsString, const char* minString, const char* const file, int line) const noexcept;
			virtual void Run() = 0;

			const std::string& GetCaseName() const noexcept { return caseName_; }
			const std::string& GetGroupName() const noexcept { return groupName_; }
			const std::string& GetSectionName() const noexcept { return sectionName_; }
			std::uintmax_t GetTestsChecked() const noexcept { return testsChecked_; }
			std::uintmax_t GetTestsPassed() const noexcept { return testsPassed_; }
			double GetCaseWeight() const noexcept { return caseWeight_; }
			std::chrono::nanoseconds GetCaseElapsed() const noexcept { return caseElapsed_; }
			void SetCaseElapsed(std::chrono::nanoseconds elapsed) { caseElapsed_ = elapsed; }
		};

	private:
		static std::unique_ptr<std::unordered_map<std::string, std::unordered_map<std::string, std::vector<TestCase*>>>> casesPtr_;
		static TestCase* currentCasePtr_;
		static std::ofstream logFile_;

		const int Run() const noexcept;
		static std::unordered_map<std::string, std::unordered_map<std::string, std::vector<TestCase*>>>& CaseMap();
		const void ReportSummary(std::uintmax_t casesTotal, std::streamsize numMaxGroupName, std::streamsize numMaxSectionName,
			std::streamsize numMaxCaseName, std::streamsize numMaxTests, std::streamsize numMaxPercent, std::streamsize numMaxWeight,
			std::streamsize numMaxStatus, std::streamsize numMaxTime) const noexcept;

	public:
		TestRegistry();
		~TestRegistry();
		static void RunAll();
		static TestCase* CurrentCase();
	};

	template <typename LHS, typename RHS>
	void TestRegistry::TestCase::CheckEqual(const LHS& lhs, const RHS& rhs, const char* lhsString, const char* rhsString, const char* const file, int line)
	{
		++testsChecked_;
		if (!(lhs == rhs)) TestRegistry::TestCase::LogCheckEqualFail(lhs, rhs, lhsString, rhsString, file, line);
		else ++testsPassed_;
	}

	template <typename LHS, typename RHS, typename Value>
	void TestRegistry::TestCase::CheckWithin(const LHS& lhs, const RHS& rhs, const Value& min, const char* lhsString, const char* rhsString, const char* minString, const char* const file, int line)
	{
		bool condition = std::abs((lhs)-(rhs)) <= std::abs(min);
		++testsChecked_;
		if (!condition) TestRegistry::TestCase::LogCheckWithinFail(lhs, rhs, min, lhsString, rhsString, minString, file, line);
		else ++testsPassed_;
	}

	template <typename LHS, typename RHS>
	void TestRegistry::TestCase::LogCheckEqualFail(const LHS& lhs, const RHS& rhs, const char* lhsString, const char* rhsString, const char* const file, int line) const noexcept
	{
		std::ostringstream oss;
		std::filesystem::path f = file;
		oss << "File: " << f.filename().string() << ", Line: " << line << ", check failed in " << TestCase::GetCaseName() << ": "
			<< "\"" << lhsString << "\" [" << lhs << "] != \"" << rhsString << "\" [" << rhs << "]\n";
		std::cout << oss.str();
		if (logFile_.is_open()) logFile_ << oss.str();
	}

	template <typename LHS, typename RHS, typename Value>
	void TestRegistry::TestCase::LogCheckWithinFail(const LHS& lhs, const RHS& rhs, const Value& min, const char* lhsString, const char* rhsString, const char* minString, const char* const file, int line) const noexcept
	{
		std::ostringstream oss;
		std::filesystem::path f = file;
		oss << "File: " << f.filename().string() << ", Line: " << line << ", check failed in " << TestCase::GetCaseName() << ": "
			<< "difference(" << lhsString << ", " << rhsString << ") > " << minString << " ==> \t|" << lhs << " - " << rhs << "| > " << std::abs(min) << '\n';
		std::cout << oss.str();
		if (logFile_.is_open()) logFile_ << oss.str();
	}

}; // end of namespace winxframe

#endif