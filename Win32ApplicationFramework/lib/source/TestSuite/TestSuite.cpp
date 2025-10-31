/*!
win32\TestSuite\TestSuite.cpp
Created: October 21, 2025
Updated: October 26, 2025
Copyright (c) 2025, Jacob Gosse

Test Suite source file.
*/

#include "TestSuite.hpp"

//////////////////////////////////////////////////
//	         TEST REGISTRY::TEST CASE	        //
//////////////////////////////////////////////////

/* CONSTRUCTOR */

TestRegistry::TestCase::TestCase(std::string const& name, std::string const& group, std::string const& section, double weight)
	: caseName_(name), groupName_(group), sectionName_(section), caseWeight_(weight)
{
	std::map<std::string, std::map<std::string, std::vector<TestCase*>>>& caseMap = TestRegistry::Cases();
	caseMap[groupName_][sectionName_].push_back(this);
}

/* FUNCTION DEFINITIONS */

void TestRegistry::TestCase::Check(bool condition, const char* const conditionString, const char* const file, int line) {
	++testsChecked_;
	if (!condition)
	{
		std::basic_ostringstream<char> oss;
		std::basic_ostream<char>& os = oss;
		std::filesystem::path f = file;
		os << f.filename().string() << " (" << line << "): error in \"" << caseName_ << "\": ";
		oss << conditionString << '\n';
		std::cout << oss.str();
		if (TestRegistry::logFile_.is_open()) TestRegistry::logFile_ << oss.str();
	}
	else ++testsPassed_;
}

void TestRegistry::TestCase::CheckMsg(bool condition, const std::string& message, const char* const file, int line) {
	++testsChecked_;
	if (!condition)
	{
		std::basic_ostringstream<char> oss;
		std::basic_ostream<char>& os = oss;
		std::filesystem::path f = file;
		os << f.filename().string() << " (" << line << "): error in \"" << caseName_ << "\": ";
		oss << message << '\n';
		std::cout << oss.str();
		if (TestRegistry::logFile_.is_open()) TestRegistry::logFile_ << oss.str();
	}
	else ++testsPassed_;
}

//////////////////////////////////////////////////
//				   TEST REGISTRY				//
//////////////////////////////////////////////////

/* CONSTRUCTOR */

TestRegistry::TestRegistry()
{
	using namespace std::string_literals;
	std::cout << "CONSTRUCTOR: TestRegistry()\n";

	std::filesystem::path filename = "test_log.txt";
	logFile_.open(filename);
	if (!logFile_) throw std::runtime_error("Could not open: "s + filename.string());

	std::cout.clear();
	std::wcout.clear();
	std::cerr.clear();
	std::wcerr.clear();
	std::cin.clear();
}

/* DESTRUCTOR */

TestRegistry::~TestRegistry()
{
	std::cout << "DESTRUCTOR: ~TestRegistry()\n";
	logFile_.close();
}

/* STATIC DEFINITIONS */

std::unique_ptr<std::map<std::string, std::map<std::string, std::vector<TestRegistry::TestCase*>>>> TestRegistry::casesPtr_ = nullptr;
TestRegistry::TestCase* TestRegistry::currentCasePtr_ = nullptr;
std::ofstream TestRegistry::logFile_;
const std::string TestRegistry::TestCase::defaultGroup_ = "default";
const std::string TestRegistry::TestCase::defaultSection_ = "default";

/* FUNCTION DEFINITIONS */

std::map<std::string, std::map<std::string, std::vector<TestRegistry::TestCase*>>>& TestRegistry::Cases()
{
	if (!casesPtr_)
	{
		casesPtr_.reset(new std::map<std::string, std::map<std::string, std::vector<TestRegistry::TestCase*>>>);
	}
	return *casesPtr_;
}

TestRegistry::TestCase* TestRegistry::CurrentCase(const char* file, int line)
{
	if (currentCasePtr_ == nullptr)
	{
		using namespace std::string_literals;
		throw std::runtime_error("Check invoked without enclosing test case: "s + file + "(" + std::to_string(line) + ")");
	}
	return currentCasePtr_;
}

void TestRegistry::RunAll()
{
	TestRegistry testRegistry;
	testRegistry.Run();
}

int TestRegistry::Run()
{
	// sort cases
	for (auto& [groupName, sectionMap] : Cases())
	{
		for (auto& [sectionName, testCases] : sectionMap)
		{
			std::sort(begin(testCases), end(testCases), [](TestCase* lhs, TestCase* rhs) { return *lhs < *rhs; });
		}
	}

	// init counts
	std::uintmax_t casesPassed{}, testsChecked{}, testsPassed{}, totalTests{};
	double score{}, maxScore{};

	// run cases
	for (auto& [groupName, sectionMap] : Cases())
	{
		for (auto& [sectionName, testCases] : sectionMap)
		{
			for (TestCase*& testCase : testCases)
			{
				currentCasePtr_ = testCase;
				auto start = std::chrono::high_resolution_clock::now();
				testCase->Run();
				totalTests = std::max(totalTests, testCase->testsChecked_);
				auto stop = std::chrono::high_resolution_clock::now();
				testCase->elapsed_ = stop - start;
				currentCasePtr_ = nullptr;
			}
		}
	}
	auto numMaxCheckedDigits = static_cast<std::streamsize>(floor(log10(totalTests))) + 1;

	// report groups
	std::cout << "=====================\n" << "| UNIT TESTS REPORT |\n" << "=====================\n";
	for (auto& [groupName, sectionMap] : Cases())
	{
		// group name
		std::cout << "Group: " << (groupName.empty() ? "(ungrouped)" : groupName) << '\n';

		// report cases
		for (auto& [sectionName, testCases] : sectionMap)
		{
			std::cout << "  Section: " << (sectionName.empty() ? "(default)" : sectionName) << '\n';

			for (TestCase*& testCase : testCases)
			{
				// check for empty test case
				bool passed = testCase->testsPassed_ == testCase->testsChecked_ && testCase->testsChecked_ > 0;

				// report pass/fail
				double ratio = double(testCase->testsPassed_) / std::max<std::uintmax_t>(1, testCase->testsChecked_);
				double percentage = ratio * 100.0;

				// time elapsed for test case
				auto elapsed = testCase->elapsed_.count();
				std::string timeUnit = "ns";
				const char* units[] = { "ns", "us", "ms", "s", "min" };
				int unitIndex = 0;

				// seconds conversion
				while (elapsed >= 1000 && unitIndex < 4)
				{
					elapsed /= 1000;
					++unitIndex;

					// minutes conversion
					if (unitIndex == 3 && elapsed >= 60)
					{
						elapsed /= 60;
						++unitIndex;
						break;
					}
				}
				timeUnit = units[unitIndex];

				std::cout << "    " << (passed ? "Passed " : "Failed ");
				std::cout << std::setw(5) << std::setprecision(1) << std::fixed << percentage << "% ";
				std::cout << "x" << testCase->caseWeight_ << " = " << std::setprecision(1) << std::fixed << ratio * testCase->caseWeight_;
				std::cout << std::setw(4) << "(" << std::setw(numMaxCheckedDigits) << testCase->testsPassed_ << "/" << std::setw(numMaxCheckedDigits) 
					<< testCase->testsChecked_ << ")";
				std::cout << " " << std::setw(4) << std::right << elapsed << ' ' << timeUnit;
				std::cout << " " << testCase->caseName_ << '\n';

				// log file
				logFile_ << std::setw(5) << std::setprecision(1) << std::fixed << ratio * testCase->caseWeight_
					<< '\t' << testCase->caseWeight_
					<< '\t' << testCase->groupName_
					<< '\t' << testCase->sectionName_
					<< '\t' << testCase->caseName_
					<< '\n';

				// enumerate overall score
				casesPassed += passed;
				testsChecked += testCase->testsChecked_;
				testsPassed += testCase->testsPassed_;
				score += ratio * testCase->caseWeight_;
				maxScore += testCase->caseWeight_;
			}
		}
	}

	// report tests passed
	double checkPercentage{ 100.0 * testsPassed / std::max<std::uintmax_t>(1, testsChecked) };
	std::basic_ostringstream<char> oss;
	oss << '\n' << std::setprecision(1) << std::fixed << testsPassed << '/' << testsChecked << " checks passed (" << checkPercentage << "%)\n";
	std::cout << oss.str();
	oss.str("");
	oss.clear();

	// iterate groups/sections and enumerate total cases
	size_t casesTotal{};
	for (const std::pair<const std::string, std::map<std::string, std::vector<TestRegistry::TestCase*>>>& groupEntry : TestRegistry::Cases())
	{
		//const std::string& groupName = groupEntry.first;
		const std::map<std::string, std::vector<TestRegistry::TestCase*>>& sectionMap = groupEntry.second;
		for (const std::pair<const std::string, std::vector<TestRegistry::TestCase*>>& sectionInfo : sectionMap)
		{
			//const std::string& sectionName = sectionEntry.first;
			const std::vector<TestRegistry::TestCase*>& testCases = sectionInfo.second;
			casesTotal += testCases.size();
		}
	}
	/*
	for (auto& [groupName, sectionMap] : Cases())
	{
		for (auto& [sectionName, caseList] : sectionMap)
		{
			casesTotal += caseList.size();
		}
	}
	*/

	oss << casesPassed << '/' << casesTotal << " cases (" << std::setprecision(1) << std::fixed << (casesTotal ? 100.0 * casesPassed / casesTotal : 0.0) << "%)\n";
	std::cout << oss.str();
	oss.str("");
	oss.clear();

	oss << "\nScore = " << score << " of " << maxScore << " (" << std::setprecision(1) << std::fixed << (maxScore > 0.0 ? score * 100 / maxScore : 0.0) << "%)";

	std::cout << oss.str() << std::endl;
	logFile_ << oss.str() << std::endl;

	return testsPassed == testsChecked ? EXIT_SUCCESS : EXIT_FAILURE;
}