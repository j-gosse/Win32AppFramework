/*!
lib\source\TestSuite\tests\math_tests.cpp
Created: October 21, 2025
Updated: October 30, 2025
Copyright (c) 2025, Jacob Gosse

Math Tests source file.
*/

#include <TestSuite/test_macros.hpp>
#include <utils/math_utils.hpp>

namespace winxframe
{
	using namespace math_utils;

	TEST_CASE(ut_math_01, "Math Tests", "Simple Calculations")
	{
		CHECK(1 + 1 == 2);
		CHECK_EQUAL(1 + 1, 2);

		constexpr double SECONDS_PER_MINUTE = 60.0;
		constexpr int MINUTES_PER_HOUR = 60;
		constexpr int HOURS_PER_DAY = 24;

		constexpr double SECONDS_PER_HOUR = MINUTES_PER_HOUR * SECONDS_PER_MINUTE;
		constexpr double SECONDS_PER_DAY = HOURS_PER_DAY * SECONDS_PER_HOUR;
		//constexpr int HOURS_PER_HALF_DAY = HOURS_PER_DAY / 2;
		//constexpr int MINUTES_PER_DAY = HOURS_PER_DAY * MINUTES_PER_HOUR;

		constexpr int secondsPerDay = 86400;

		static_assert(SECONDS_PER_DAY == secondsPerDay);
		CHECK(SECONDS_PER_DAY == secondsPerDay);
	}

	TEST_CASE(ut_math_02, "Math Tests", "Absolute Values")
	{
		/* Abs */
		static_assert(Abs(0) == 0);
		static_assert(Abs(1) == 1);
		static_assert(Abs(-5) == 5);
		static_assert(Abs(5u) == 5u);
		static_assert(Abs(std::numeric_limits<double>::infinity()) == std::numeric_limits<double>::infinity());
		static_assert(Abs(-std::numeric_limits<double>::infinity()) == std::numeric_limits<double>::infinity());
		constexpr int minInt = std::numeric_limits<int>::min();
		constexpr int result = Abs(minInt);
		CHECK(result == std::numeric_limits<int>::max());
		CHECK(std::isnan(Abs(std::numeric_limits<float>::quiet_NaN())));

		double negZero = -0.0;
		float negZeroF = -0.0f;
		CHECK(std::signbit(negZero));
		CHECK(Abs(negZero) == 0.0);
		CHECK(!std::signbit(Abs(negZero)));
		CHECK(std::signbit(negZeroF));
		CHECK(Abs(negZeroF) == 0.0f);
		CHECK(!std::signbit(Abs(negZeroF)));

		int absCheckIntValues[] = { 0, 1, -1, 5, -5, 42, -42 };
		for (int x : absCheckIntValues)
			CHECK(Abs(x) == (x < 0 ? -x : x));
		double absCheckDoubleValues[] = { 0.5, 1.0, -1.0, 5.5, -5.5, -3.14 };
		for (double x : absCheckDoubleValues)
			CHECK(Abs(x) == (x < 0 ? -x : x));
		float absCheckFloatValues[] = { 0.9f, 1.0f, -1.0f, 5.9f, -5.1f, -3.14f };
		for (float x : absCheckFloatValues)
			CHECK(Abs(x) == (x < 0 ? -x : x));
	}

	TEST_CASE(ut_math_03, "Math Tests", "Modulus")
	{
		/* Mod */
		static_assert(std::is_same_v<decltype(Mod(5, 3)), int>);
		static_assert(std::is_same_v<decltype(Mod(5LL, 3LL)), long long>);
		static_assert(std::is_same_v<decltype(Mod(5u, 3u)), unsigned>);
		static_assert(Mod(0, 1) == 0);
		static_assert(Mod(-5, -3) == 1);
		static_assert(Mod(-5, 3) == 1);
		static_assert(Mod(5, -3) == 2);
		static_assert(Mod(5, 3) == 2);
		int modCheckIntValues[][2] = { {0, 1}, {0, -5}, {5, 3}, {5, -3}, {-5, 3}, {-5, -3} };
		int modExpected[] = { 0, 0, 2, 2, 1, 1 };
		for (int i = 0; i < 6; ++i)
			CHECK(Mod(modCheckIntValues[i][0], modCheckIntValues[i][1]) == modExpected[i]);

		constexpr long long largeA = 9'223'372'036'854'775'807LL;
		constexpr long long largeB = 9'223'372'036'854'775'000LL;
		CHECK(Mod(largeA, 97LL) == largeA % 97);
		CHECK(Mod(largeB, 123456789LL) == largeB % 123456789);
		CHECK(Mod(-largeA, 97LL) == ((-largeA) % 97 + 97) % 97);
		CHECK(Mod(std::numeric_limits<long long>::max(), 97LL) == std::numeric_limits<long long>::max() % 97);
		CHECK(Mod(std::numeric_limits<long long>::min() + 1, 97LL) == ((std::numeric_limits<long long>::min() + 1) % 97 + 97) % 97);

		//assert(Mod(1, 0) && "division by zero");
	}

	TEST_CASE(ut_math_04, "Math Tests", "Natural Logarithm")
	{
		/* Log */
		static_assert(Abs(Log(1.0) - 0.0) < 1e-12);
		static_assert(Abs(Log(2.718281828459045) - 1.0) < 1e-9);
		static_assert(Abs(Log(7.38905609893065) - 2.0) < 1e-8);
		static_assert(Abs(Log(0.36787944117144233) + 1.0) < 1e-9);
		static_assert(Abs(Log(2.7182818f) - 1.0f) < 1e-5f);
		CHECK(Log(1.0) == 0.0);
		CHECK(Log(7.38905609893065) > 1.9999);
		CHECK(Log(0.25) < -1.0);
		double x = 2.0, y = 3.0;
		CHECK(Log(x * y) - (Log(x) + Log(y)) < 1e-12);
		CHECK(Abs(Log(10.0) - std::log(10.0)) < 1e-8);

		CHECK(Abs(Log(Exp(5.0)) - 5.0) < 1e-8);
		CHECK(Abs(Exp(Log(9.0)) - 9.0) < 1e-8);

		//assert(Log(0.0) && "domain: log(0)");
		//assert(Log(-1.0) && "domain: negative input");
	}

	TEST_CASE(ut_math_05, "Math Tests", "Exponentiation")
	{
		/* Exp */
		static_assert(Abs(Exp(0.0) - 1.0) < 1e-12);
		static_assert(Abs(Exp(1.0) - 2.718281828459045) < 1e-9);
		static_assert(Abs(Exp(2.0) - 7.38905609893065) < 1e-8);
		static_assert(Abs(Exp(-1.0) - 0.36787944117144233) < 1e-9);
		static_assert(Abs(Exp(1.0f) - 2.7182818f) < 1e-5f);
		CHECK(Exp(-800.0) == 0.0);
		CHECK(Exp(800.0) == std::numeric_limits<double>::infinity());
		CHECK(Exp(710.0) == std::numeric_limits<double>::infinity());
		CHECK(Exp(-750.0) == 0.0);
		CHECK(Abs(Exp(3.0) - std::exp(3.0)) < 1e-8);
	}

	TEST_CASE(ut_math_06, "Math Tests", "Floor Division")
	{
		/* FloorDiv */
		static_assert(FloorDiv(0, 5) == 0);
		static_assert(FloorDiv(7, 3) == 2);
		CHECK(FloorDiv(0, 3) == 0);
		CHECK(FloorDiv(0, 5) == 0);
		CHECK(FloorDiv(0, -7) == 0);
		CHECK(FloorDiv(0, 7) == 0);
		CHECK(FloorDiv(7, 3) == 2);
		CHECK(FloorDiv(-7, -3) == 2);
		CHECK(FloorDiv(8u, 3u) == 2u);
		CHECK(FloorDiv(7, -3) == -3);
		CHECK(FloorDiv(-7, 3) == -3);

		//assert(FloorDiv(1, 0) && "division by zero");
	}

	TEST_CASE(ut_math_07, "Math Tests", "Greatest Common Divisor")
	{
		/* Greatest Common Divisor */
		static_assert(GCD(0, 0) == 0);
		CHECK(GCD(0, 0) == 0);
		CHECK(GCD(10, 0) == 10);
		CHECK(GCD(0, 10) == 10);
		CHECK(GCD(-24, 18) == 6);
		CHECK(GCD(12, 18) == 6);
		CHECK(GCD(-12, 18) == 6);
	}

	TEST_CASE(ut_math_08, "Math Tests", "Least Common Multiple")
	{
		/* Least Common Multiple */
		static_assert(LCM(0, 5) == 0);
		CHECK(LCM(0, 5) == 0);
		CHECK(LCM(5, 0) == 0);
		CHECK(LCM(-4, 6) == 12);
		CHECK(LCM(4, 6) == 12);
		CHECK(LCM(-12, 18) == 36);
		CHECK(LCM(12, 18) == 36);
	}

	TEST_CASE(ut_math_09, "Math Tests", "Clamp")
	{
		/* Clamp */
		static_assert(Clamp(-3, 0, 10) == 0);
		CHECK(Clamp(-3, 0, 10) == 0);
		CHECK(Clamp(0, 0, 10) == 0);
		CHECK(Clamp(5, 0, 10) == 5);
		CHECK(Clamp(10, 0, 10) == 10);
		CHECK(Clamp(15, 0, 10) == 10);
	}

	TEST_CASE(ut_math_10, "Math Tests", "Lesser of Two Values")
	{
		/* Min */
		static_assert(Min(1, 2) == 1);
		CHECK(Min(1, 2) == 1);
		CHECK(Min(5, 2, 7, 1, 3) == 1);
		CHECK(Min(5, 2, 8, 7) == 2);
		CHECK(Min(3, 3, 3, 3) == 3);
	}

	TEST_CASE(ut_math_11, "Math Tests", "Greater of Two Values")
	{
		/* Max */
		static_assert(Max(3, 4) == 4);
		CHECK(Max(3, 4) == 4);
		CHECK(Max(5, 2, 7, 1, 3) == 7);
		CHECK(Max(7, 7, 7, 7) == 7);
		CHECK(Max(5, 2, 8, 7) == 8);
	}

	TEST_CASE(ut_math_12, "Math Tests", "Factorial")
	{
		/* Factorial */
		static_assert(Factorial(0) == 1);
		static_assert(Factorial(1) == 1);
		static_assert(Factorial(19) == 121'645'100'408'832'000ULL);
		static_assert(Factorial(20) == 2'432'902'008'176'640'000ULL);
		unsigned long long f = 1;
		for (int n = 1; n <= 20; ++n)
		{
			f *= n;
			CHECK(f == Factorial(n));
		}
	}

	TEST_CASE(ut_math_13, "Math Tests", "Modulus Inverse")
	{
		/* ModInverse */
		static_assert(ModInverse(1, 19) == 1);
		static_assert(ModInverse(3, 11) == 4);
		CHECK(ModInverse(1, 19) == 1);
		CHECK(ModInverse(7, 13) == 2);
		CHECK(ModInverse(2, 5) == 3);
		CHECK(ModInverse(3, 11) == 4);
		CHECK(ModInverse(3, 7) == 5);
		CHECK(ModInverse(10, 17) == 12);

		constexpr unsigned long long bigPrime = 1'000'000'007ULL;
		CHECK(ModInverse(3ULL, bigPrime) == 333'333'336ULL);
		CHECK(ModInverse(7ULL, bigPrime) == 142'857'144ULL);
		CHECK(ModInverse(1234567ULL, bigPrime) == 989'145'189ULL);
		for (unsigned long long a : {2ULL, 5ULL, 123456789ULL, 987654321ULL})
			CHECK((a * ModInverse(a, bigPrime)) % bigPrime == 1ULL);
		for (unsigned long long a = 1; a <= 100; ++a)
		{
			auto inv = ModInverse(a, bigPrime);
			CHECK((a * inv) % bigPrime == 1ULL);
		}
	}

	TEST_CASE(ut_math_14, "Math Tests", "Modulus Binomial")
	{
		/* ModBinomialSmall */
		static_assert(ModBinomialSmall(8, 0, 17) == 1);
		CHECK(ModBinomialSmall(8, 0, 17) == 1);
		CHECK(ModBinomialSmall(8, 8, 17) == 1);
		CHECK(ModBinomialSmall(5, 2, 7) == 3);
		CHECK(ModBinomialSmall(10, 5, 13) == 5);
		CHECK(ModBinomialSmall(6, 3, 11) == 9);
	}

	TEST_CASE(ut_math_15, "Math Tests", "Modulus Binomial")
	{
		/* ModBinomial */
		static_assert(ModBinomial(5, 6, 7) == 0);
		static_assert(ModBinomial(0, 0, 7) == 1);
		static_assert(ModBinomial(5, 2, 7) == 3);
		CHECK(ModBinomial(5, 6, 7) == 0);
		CHECK(ModBinomial(0, 0, 7) == 1);
		CHECK(ModBinomial(5, 0, 7) == 1);
		CHECK(ModBinomial(5, 5, 7) == 1);
		CHECK(ModBinomial(5, 2, 7) == 3);
		CHECK(ModBinomial(6, 3, 11) == 9);
		CHECK(ModBinomial(10, 5, 13) == 5);
		CHECK(ModBinomial(66, 33, 1'000'000'007ULL) == 480'267'059ULL);
	}

	TEST_CASE(ut_math_16, "Math Tests", "Exponentiation")
	{
		/* Powers */
		static_assert(Pow(0, 1) == 0);
		static_assert(Pow(0, 5) == 0);
		static_assert(Pow(0, 0) == 1);
		static_assert(Pow(2, 0) == 1);
		static_assert(Pow(-2, 3) == -8);
		static_assert(static_cast<int>(Pow(2.0, 3)) == 8);
		static_assert(Pow(-2, 4) == 16);
		static_assert(static_cast<int>(Pow(2.0f, 4)) == 16);
		static_assert(Pow(5LL, 3) == 125LL);
		static_assert(Pow(2ULL, 10) == 1024ULL);
		for (long long b = -20; b <= 20; ++b)
		{
			for (int e = 0; e <= 14; ++e)
			{
				long long p = Pow<long long>(b, e);
				long long expectedLL = 1;
				for (int i = 0; i < e; ++i)
				{
					expectedLL *= b;
				}
				CHECK(p == expectedLL);
			}
		}
		for (unsigned long long b = 0; b <= 20; ++b)
		{
			for (int e = 0; e <= 14; ++e)
			{
				unsigned long long p = Pow<unsigned long long>(b, e);
				unsigned long long expectedULL = 1;
				for (int i = 0; i < e; ++i)
				{
					expectedULL *= b;
				}
				CHECK(p == expectedULL);
			}
		}

		//assert(Pow(2, 31) && "should trigger overflow assert");
	}

	TEST_CASE(ut_math_17, "Math Tests", "Fibonacci")
	{
		/* Fibonacci */
		static_assert(Fibonacci(0) == 0);
		static_assert(Fibonacci(1) == 1);
		static_assert(Fibonacci(93) == 12'200'160'415'121'876'738ULL);
		for (int n = 0; n <= 93; ++n)
		{
			unsigned long long fn = Fibonacci(n);
			if (n >= 2) CHECK(fn == Fibonacci(n - 1) + Fibonacci(n - 2));
			CHECK(fn <= std::numeric_limits<unsigned long long>::max());
		}

		//assert(Fibonacci(94) && "should trigger overflow assert");
	}

	TEST_CASE(ut_math_18, "Math Tests", "Summation")
	{
		/* Sum */
		static_assert(Sum(0) == 0);
		static_assert(Sum(1) == 1);
		static_assert(Sum(46340) > 0);
		static_assert(Sum(46340) == (46340LL * (46340LL + 1)) / 2);
		for (int n = 0; n <= 46340; ++n)
		{
			int s = Sum(n);
			assert(s >= 0);
			if (n > 0) CHECK(s == Sum(n - 1) + n);
		}
		for (unsigned long long n = 0; n <= 3037000499ULL; n += 1'000'000ULL)
		{
			unsigned long long s = Sum(n);
			CHECK(s >= 0);
		}

		//assert(Sum(46341) && "should trigger overflow assert");
	}

	TEST_CASE(ut_math_19, "Math Tests", "Summation")
	{
		/* SumSquares */
		static_assert(SumSquares(0) == 0);
		static_assert(SumSquares(1) == 1);
		static_assert(SumSquares(181) > 0);
		static_assert(SumSquares(181) == (181LL * (181LL + 1) * (2LL * 181 + 1)) / 6);
		for (int n = 0; n <= 181; ++n)
		{
			int ss = SumSquares(n);
			assert(ss >= 0);
			if (n > 0) CHECK(ss == SumSquares(n - 1) + n * n);
		}
		for (unsigned long long n = 0; n <= 30303ULL; ++n)
		{
			unsigned long long ss = SumSquares(n);
			CHECK(ss >= 0);
		}
	}
}; // end of namespace winxframe