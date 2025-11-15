/*!
lib\source\TestSuite\TestSuite.cpp
Created: October 21, 2025
Updated: November 15, 2025
Copyright (c) 2025, Jacob Gosse

Test Suite source file.
*/

#include <TestSuite/TestSuite.hpp>
#include <win32/Error/error_macros.hpp>
#include <win32/Console/ConsoleColor.hpp>
#include <utils/string_utils.hpp>
#include <utils/stream_utils.hpp>
#include <utils/container_utils.hpp>
#include <utils/time_utils.hpp>

namespace winxframe
{
	//////////////////////////////////////////////////
	//	         TEST REGISTRY::TEST CASE	        //
	//////////////////////////////////////////////////

	/* CONSTRUCTOR */

	TestRegistry::TestCase::TestCase(const std::string& name, const std::string& group, const std::string& section, double weight) :
		caseName_(name), 
		groupName_(group), 
		sectionName_(section), 
		caseWeight_(weight)
	{
		try
		{
			TestRegistry::CaseMap()[groupName_][sectionName_].push_back(this);
		}
		catch (const Error& e)
		{
			e.Log();
			e.MsgBox();
		}
	}

	/* FUNCTION DEFINITIONS */

	void TestRegistry::TestCase::Check(bool condition, const char* const conditionString, const char* const file, int line)
	{
		++testsChecked_;
		if (!condition)
			this->LogCheckFail(conditionString, file, line);
		else
			++testsPassed_;
	}

	void TestRegistry::TestCase::Check(bool condition, const std::string& message, const char* const file, int line)
	{
		++testsChecked_;
		if (!condition)
			this->LogCheckFail(message, file, line);
		else
			++testsPassed_;
	}

	void TestRegistry::TestCase::LogCheckFail(const char* const conditionString, const char* const file, int line) const noexcept
	{
		std::ostringstream oss;
		std::filesystem::path f = file;
		oss << "File: " << f.filename().string() << ", Line: " << line << ", check failed in " << this->GetCaseName() << ": " << conditionString << '\n';
		std::cout << oss.str();
		if (TestRegistry::logFile_.is_open())
			TestRegistry::logFile_ << oss.str();
	}

	void TestRegistry::TestCase::LogCheckFail(const std::string& message, const char* const file, int line) const noexcept
	{
		std::ostringstream oss;
		std::filesystem::path f = file;
		oss << "File: " << f.filename().string() << ", Line: " << line << ", check failed in " << this->GetCaseName() << ", with custom message: " << message << '\n';
		std::cout << oss.str();
		if (TestRegistry::logFile_.is_open())
			TestRegistry::logFile_ << oss.str();
	}

	//////////////////////////////////////////////////
	//				   TEST REGISTRY				//
	//////////////////////////////////////////////////

	/* CONSTRUCTOR */

	TestRegistry::TestRegistry()
		try :
		isCleaned_(false)
	{
		std::cout << "CONSTRUCTOR: TestRegistry()\n";

		std::filesystem::path filename = "test_log.txt";
		TestRegistry::logFile_.open(filename);
		if (!TestRegistry::logFile_)
		{
			std::wostringstream woss;
			woss << L"TestRegistry::TestRegistry() : Log File could not be opened: " << filename.wstring();
			THROW_ERROR_CTX(woss.str());
		}

		std::cout.clear();
		std::wcout.clear();
		std::cerr.clear();
		std::wcerr.clear();
	}
	catch (const Error&)
	{
		this->Cleanup();
		RETHROW_ERROR_CTX(L"Rethrowing TestRegistry constructor error!");
	}

	/* DESTRUCTOR */

	TestRegistry::~TestRegistry()
	{
		std::cout << "DESTRUCTOR: ~TestRegistry()\n";
		OutputDebugStringW(L"DESTRUCTOR: ~TestRegistry()\n");
		this->Cleanup();
	}

	/* STATIC DEFINITIONS */

	std::unique_ptr<std::unordered_map<std::string, std::unordered_map<std::string, std::vector<TestRegistry::TestCase*>>>> TestRegistry::casesPtr_ = nullptr;
	TestRegistry::TestCase* TestRegistry::currentCasePtr_ = nullptr;
	std::ofstream TestRegistry::logFile_;

	/* FUNCTION DEFINITIONS */

	std::unordered_map<std::string, std::unordered_map<std::string, std::vector<TestRegistry::TestCase*>>>& TestRegistry::CaseMap()
	{
		if (!TestRegistry::casesPtr_)
			TestRegistry::casesPtr_.reset(new std::unordered_map<std::string, std::unordered_map<std::string, std::vector<TestRegistry::TestCase*>>>);
		return *TestRegistry::casesPtr_;
	}

	TestRegistry::TestCase* TestRegistry::CurrentCase()
	{
		if (!TestRegistry::currentCasePtr_) throw std::runtime_error("CurrentCase: improperly invoked test case");
		return TestRegistry::currentCasePtr_;
	}

	void TestRegistry::RunAll()
	{
		TestRegistry testRegistry;
		testRegistry.Run();
	}

	const int TestRegistry::Run() const noexcept
	{
		std::uintmax_t testsTotal{}, casesTotal{};
		std::streamsize numMaxGroupName{ 11 }, numMaxSectionName{ 13 }, numMaxCaseName{ 10 }, numMaxTests{ 11 }, numMaxPercent{ 6 }, 
			numMaxWeight{ 3 }, numMaxStatus{ 4 }, numMaxTime{ 8 };
		std::chrono::time_point<std::chrono::high_resolution_clock> start, stop;

		for (auto& [groupName, sectionMap] : TestRegistry::CaseMap())
		{
			for (auto& [sectionName, testCases] : sectionMap)
			{
				for (TestCase* testCase : testCases)
				{
					TestRegistry::currentCasePtr_ = testCase;

					start = std::chrono::high_resolution_clock::now();
					testCase->Run();
					testsTotal = std::max(testsTotal, testCase->GetTestsChecked());
					stop = std::chrono::high_resolution_clock::now();
					testCase->SetCaseElapsed(stop - start);

					casesTotal += 1;
					numMaxGroupName = stream_utils::MaxStreamSize(testCase->GetGroupName().size(), numMaxGroupName);
					numMaxSectionName = stream_utils::MaxStreamSize(testCase->GetSectionName().size(), numMaxSectionName);
					numMaxCaseName = stream_utils::MaxStreamSize(testCase->GetCaseName().size(), numMaxCaseName);
					numMaxWeight = stream_utils::MaxStreamSize(testCase->GetCaseWeight(), numMaxWeight);

					TestRegistry::currentCasePtr_ = nullptr;
				}
			}
		}
		numMaxTests = testsTotal == 0 ? numMaxTests : static_cast<std::streamsize>(floor(log10(testsTotal))) + 1;
		this->ReportSummary(casesTotal, numMaxGroupName, numMaxSectionName, numMaxCaseName, numMaxTests, numMaxPercent, numMaxWeight, numMaxStatus, numMaxTime);

		return EXIT_SUCCESS;
	}

	const void TestRegistry::ReportSummary(std::uintmax_t casesTotal, std::streamsize numMaxGroupName, std::streamsize numMaxSectionName,
		std::streamsize numMaxCaseName, std::streamsize numMaxTests, std::streamsize numMaxPercent, std::streamsize numMaxWeight, 
		std::streamsize numMaxStatus, std::streamsize numMaxTime) const noexcept
	{
		std::ostringstream oss;

		std::streamsize tableWidth = numMaxGroupName + numMaxSectionName + numMaxCaseName + ((numMaxTests * 2) + 1) + numMaxPercent + ((numMaxWeight * 2) + 1) + numMaxStatus + numMaxTime + 25;
		std::streamsize titleWidth = tableWidth - 4;
		std::uintmax_t rowsTotal = casesTotal + 6;
		std::uintmax_t casesPassed{}, testsChecked{}, testsPassed{};
		double score{}, maxScore{};
		std::chrono::nanoseconds casesTotalElapsed{};

		oss.str().reserve(tableWidth * rowsTotal);

		oss << console_color::BrightWhite
			<< std::left
			<< std::string(tableWidth, '=') << '\n'
			<< "| " << string_utils::CenterText("UNIT TESTS REPORT", static_cast<int>(titleWidth)) << " |" << '\n'
			<< std::string(tableWidth, '=') << '\n'
			<< "| " << std::setw(numMaxGroupName) << "GROUP NAME"
			<< " | " << std::setw(numMaxSectionName) << "SECTION NAME"
			<< " | " << std::setw(numMaxCaseName) << "CASE NAME"
			<< " | " << std::setw((numMaxTests * 2) + 1) << "# OF TESTS"
			<< " | " << std::setw(numMaxPercent) << "RATIO"
			<< " | " << std::setw((numMaxWeight * 2) + 1) << "GRADE"
			<< " | " << std::setw(numMaxStatus) << "STAT"
			<< " | " << std::setw(numMaxTime) << "TIME"
			<< " |" << '\n' << std::string(tableWidth, '-') << '\n';
		std::cout << oss.str();
		oss.str("");
		oss.clear();

		for (const auto& groupName : container_utils::SortedMapKeys(CaseMap()))
		{
			const auto& sectionMap = CaseMap().at(groupName);

			// collect section names
			std::vector<std::string> sectionKeys;
			sectionKeys.reserve(sectionMap.size());
			for (const auto& [sectionName, _] : sectionMap)
				sectionKeys.push_back(sectionName);

			// sort sections by the CaseName of the first test case in each section
			std::sort(sectionKeys.begin(), sectionKeys.end(),
				[&](const std::string& a, const std::string& b)
				{
					const auto& vecA = sectionMap.at(a);
					const auto& vecB = sectionMap.at(b);
					return vecA.front()->GetCaseName() < vecB.front()->GetCaseName();
				}
			);

			for (const auto& sectionName : sectionKeys)
			{
				const auto& testCases = sectionMap.at(sectionName);

				for (auto* testCase : testCases)
				{
					bool passed = testCase->GetTestsPassed() == testCase->GetTestsChecked() && testCase->GetTestsChecked() > 0;
					double ratio = double(testCase->GetTestsPassed()) / std::max<std::uintmax_t>(1, testCase->GetTestsChecked());
					double percentage = ratio * 100.0;
					casesTotalElapsed += testCase->GetCaseElapsed();
					time_utils::double_time caseElapsed = time_utils::ConvertToDouble(testCase->GetCaseElapsed());

					std::ostringstream strPercentage;
					strPercentage << std::setprecision(1) << std::fixed << percentage << '%';
					std::ostringstream strCaseElapsed;
					strCaseElapsed << std::setprecision(1) << std::fixed << caseElapsed.time << ' ' << caseElapsed.unit;

					oss << console_color::BrightWhite
						<< "| " << std::setw(numMaxGroupName) << (groupName.empty() ? "ungrouped" : groupName)
						<< " | " << std::setw(numMaxSectionName) << (sectionName.empty() ? "default" : sectionName)
						<< " | " << std::setw(numMaxCaseName) << testCase->GetCaseName()
						<< " | " << std::setw(numMaxTests) << testCase->GetTestsPassed() << '/' << std::setw(numMaxTests) << testCase->GetTestsChecked()
						<< " | " << std::setw(numMaxPercent) << strPercentage.str()
						<< " | " << std::setw(numMaxWeight) << std::setprecision(1) << std::fixed << (ratio * testCase->GetCaseWeight()) << '/' << std::setw(numMaxWeight) << testCase->GetCaseWeight()
						<< " | ";
					std::cout << oss.str();
					oss.str("");
					oss.clear();

					oss << (passed ? console_color::WhiteOnGreen : console_color::WhiteOnRed) << std::setw(numMaxStatus) << (passed ? "PASS" : "FAIL");
					std::cout << oss.str();
					oss.str("");
					oss.clear();

					oss << console_color::BrightWhite << " | " << std::setw(numMaxTime) << strCaseElapsed.str() << " |" << "\n";
					std::cout << oss.str();
					oss.str("");
					oss.clear();

					TestRegistry::logFile_ << std::setw(5) << std::setprecision(1) << std::fixed << ratio * testCase->GetCaseWeight()
						<< '\t' << testCase->GetCaseWeight()
						<< '\t' << testCase->GetGroupName()
						<< '\t' << testCase->GetSectionName()
						<< '\t' << testCase->GetCaseName()
						<< '\n';

					casesPassed += passed;
					testsChecked += testCase->GetTestsChecked();
					testsPassed += testCase->GetTestsPassed();
					score += ratio * testCase->GetCaseWeight();
					maxScore += testCase->GetCaseWeight();
				}
			}
		}
		oss << std::string(tableWidth, '-') << "\n";
		std::cout << oss.str();
		oss.str("");
		oss.clear();

		time_utils::double_time totalElapsed = time_utils::ConvertToDouble(casesTotalElapsed);
		double checkPercentage{ 100.0 * testsPassed / std::max<std::uintmax_t>(1, testsChecked) };

		oss << '\n' << "Total Running Time: " << std::setprecision(2) << std::fixed << totalElapsed.time << ' ' << totalElapsed.unit
			<< '\n' << std::setprecision(1) << std::fixed << testsPassed << '/' << testsChecked << " tests (" << checkPercentage << "%)\n"
			<< casesPassed << '/' << casesTotal << " cases (" << std::setprecision(1) << std::fixed << (casesTotal ? 100.0 * casesPassed / casesTotal : 0.0) << "%)\n"
			<< score << " of " << maxScore << " score (" << std::setprecision(1) << std::fixed << (maxScore > 0.0 ? score * 100 / maxScore : 0.0) << "%)\n";
		std::cout << oss.str() << std::endl;
		TestRegistry::logFile_ << oss.str() << std::endl;
		std::cout << console_color::Default;
		oss.str("");
		oss.clear();
	}

	void TestRegistry::Cleanup()
	{
		if (isCleaned_) return;

		if (TestRegistry::logFile_)
			TestRegistry::logFile_.close();

		if (TestRegistry::currentCasePtr_)
		{
			delete TestRegistry::currentCasePtr_;
			TestRegistry::currentCasePtr_ = nullptr;
		}

		if (TestRegistry::casesPtr_)
		{
			TestRegistry::casesPtr_.reset();
			TestRegistry::casesPtr_ = nullptr;
		}

		std::cout << console_color::Default;
		std::cout.clear();
		std::wcout.clear();
		std::cerr.clear();
		std::wcerr.clear();

		isCleaned_ = true;
	}
}; // end of namespace winxframe