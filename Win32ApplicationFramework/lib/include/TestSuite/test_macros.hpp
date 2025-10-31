/*!
lib\include\TestSuite\test_macros.hpp
Created: October 21, 2025
Updated: October 30, 2025
Copyright (c) 2025, Jacob Gosse

Test Macros header file.
*/

#pragma once

#ifndef TEST_MACROS_HPP_
#define TEST_MACROS_HPP_

#include "TestSuite.hpp"

namespace winxframe
{
	#define CHECK(condition) TestRegistry::CurrentCase()->Check((condition), #condition)

	#define MSG_CHECK(condition, message) TestRegistry::CurrentCase()->Check((condition), #message)

	#define CHECK_EQUAL(testValue, expectedValue) TestRegistry::CurrentCase()->CheckEqual((testValue), (expectedValue), #testValue, #expectedValue)

	#define CHECK_WITHIN(testValue, expectedValue, minimum) TestRegistry::CurrentCase()->CheckWithin((testValue), (expectedValue), (minimum), #testValue, #expectedValue, #minimum)

	#define TEST_CASE(name, ...)\
		static class TestCase_##name : public TestRegistry::TestCase {\
		public: TestCase_##name() : TestCase(#name, __VA_ARGS__) { }\
		public: virtual void Run() override;\
		} TestCase_##name##_g;\
		void TestCase_##name::Run()
	
	#define TEST_CASE_WEIGHTED(name, weight)\
		static class TestCase_##name : public TestRegistry::TestCase {\
		public: TestCase_##name() : TestCase(#name, weight) {}\
		public: virtual void Run() override;\
		} TestCase_##name##_g;\
		void TestCase_##name::Run()
	
	#define TEST_CASE_PERCENTAGE(name, percent) \
		static class TestCase_##name : public TestRegistry::TestCase {\
		public: TestCase_##name() : TestCase(#name, percent) {}\
		public: virtual void Run() override;\
		} TestCase_##name##_g;\
		void TestCase_##name::Run()
	
	#define TEST_CASE_GROUPED(name, group, weight)\
		static class TestCase_##name : public TestRegistry::TestCase {\
		public: TestCase_##name() : TestCase(#name, group, weight) {}\
		public: virtual void Run() override;\
		} TestCase_##name##_g;\
		void TestCase_##name::Run()
	
	#define TEST_CASE_SECTIONED(name, group, section)\
		static class TestCase_##name : public TestRegistry::TestCase {\
		public: TestCase_##name() : TestCase(#name, group, section) {}\
		public: virtual void Run() override;\
		} TestCase_##name##_g;\
		void TestCase_##name::Run()
	
	#define TEST_CASE_SECTIONED_WEIGHTED(name, group, section, weight)\
		static class TestCase_##name : public TestRegistry::TestCase {\
		public: TestCase_##name() : TestCase(#name, group, section, weight) {}\
		public: virtual void Run() override;\
		} TestCase_##name##_g;\
		void TestCase_##name::Run()
}; // end of namespace winxframe

#endif