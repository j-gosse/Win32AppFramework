/*!
lib\include\utils\math_utils.hpp
Created: October 24, 2025
Updated: October 30, 2025
Copyright (c) 2025, Jacob Gosse

Math Utilities header file.
*/

#pragma once

#ifndef MATH_UTILS_HPP_
#define MATH_UTILS_HPP_

#include <cmath>
#include <cassert>
#include <climits>
#include <concepts>

namespace winxframe
{
    namespace math_utils
    {
        template <typename T>
        concept arithmetic = std::integral<T> || std::floating_point<T>;

    #pragma region MATHEMATICAL HELPERS
        /* Replace the value of an object with a new value and return the object's original value */
        template <typename T, typename U = T>
        [[nodiscard]] constexpr T Exchange(T& obj, U&& newVal) noexcept(std::is_nothrow_assignable_v<T&, U&&>)
        {
            T originalVal = std::move(obj);
            obj = std::forward<U>(newVal);
            return originalVal;
        }

        /* Returns the absolute value of a number */
        template <arithmetic T>
        [[nodiscard]] constexpr T Abs(T x) noexcept
        {
            if constexpr (std::is_integral_v<T> && std::is_signed_v<T>)
                return x == std::numeric_limits<T>::min() ? std::numeric_limits<T>::max() : (x < 0 ? -x : x);
            else if constexpr (std::is_floating_point_v<T>)
            {
                if (x == 0.0) return T(0);
                return x < 0 ? -x : x;
            }
            else
                return x;
        }

        /* Calculates a mathematical modulus (always non-negative) */
        template <std::integral T>
        [[nodiscard]] constexpr T Mod(T a, T b) noexcept
        {
            assert(b != 0 && "Mod: division by zero");
            T r = a % b;
            if constexpr (std::is_signed_v<T>)
                return (r < 0) ? (r + Abs(b)) : r;
            else
                return r;
        }

        /* Calculates e^x (the base-e exponential), where e approx. 2.7182818 */
        template<std::floating_point T>
        [[nodiscard]] constexpr T Exp(T x) noexcept
        {
            constexpr T maxExp = std::is_same_v<T, float> ? 88.722839f : std::is_same_v<T, long double> ? 11356.523406841f : 709.782712893384;
            constexpr T minExp = std::is_same_v<T, float> ? -103.27893f : std::is_same_v<T, long double> ? -11392.0L : -745.133219101941;

            if (x >= maxExp)
                return std::numeric_limits<T>::infinity();
            if (x <= minExp)
                return static_cast<T>(0);

            T term = static_cast<T>(1);
            T sum = static_cast<T>(1);

            for (int n = 1; n < 100; ++n)
            {
                term *= x / static_cast<T>(n);
                sum += term;

                if (term < static_cast<T>(1e-12) && term > static_cast<T>(-1e-12))
                    break;
            }

            return sum;
        }

        /* Computes the natural logarithm (ln (x)), which is the inverse of the exponential function e^x */
        template<std::floating_point T>
        [[nodiscard]] constexpr T Log(T x) noexcept
        {
            assert(x > static_cast<T>(0) && "Log: input must be positive");
            if (x == static_cast<T>(1)) return static_cast<T>(0);
            if (x == static_cast<T>(0)) return -std::numeric_limits<T>::infinity();
            if (x < static_cast<T>(0)) return std::numeric_limits<T>::quiet_NaN();

            // range reduction, bring x into [0.5, 2]
            int k = 0;
            while (x > static_cast<T>(2))
            {
                x /= static_cast<T>(2); ++k;
            }
            while (x < static_cast<T>(0.5))
            {
                x *= static_cast<T>(2); --k;
            }

            // series expansion for ln(1 + y)
            T y = x - static_cast<T>(1);
            T term = y;
            T sum = y;

            for (int n = 2; n < 100; ++n)
            {
                term *= -y;
                T add = term / static_cast<T>(n);
                sum += add;

                if (add < static_cast<T>(1e-12) && add > static_cast<T>(-1e-12))
                    break;
            }

            // precomputed ln(2) constants
            constexpr T LN2 = std::is_same_v<T, float> ? static_cast<T>(0.6931471805599453f) :
                std::is_same_v<T, long double> ? static_cast<T>(0.693147180559945309417232121458176568L) :
                static_cast<T>(0.69314718055994530942);

            // adjust for power of two normalization
            return sum + static_cast<T>(k) * LN2;
        }

        /* Calculates floor(a / b), i.e., the largest integer <= a / b */
        template <std::integral T>
        [[nodiscard]] constexpr T FloorDiv(T a, T b) noexcept
        {
            assert(b != 0 && "FloorDiv: division by zero");
            if constexpr (std::is_signed_v<T>)
            {
                T q = a / b;
                T r = a % b;
                if ((r != 0) && ((r < 0) != (b < 0))) --q;
                return q;
            }
            else
                return a / b;
        }
    #pragma endregion

    #pragma region GREATEST COMMON DIVISOR
        /* Calculates the largest positive integer that divides two or more integers without leaving a remainder */
        template <std::integral T>
        [[nodiscard]] constexpr T GCD(T a, T b) noexcept
        {
            while (b != 0) b = Exchange(a, b) % b;
            if constexpr (std::is_signed_v<T>)
                return a < 0 ? -a : a;
            else
                return a;
        }
    #pragma endregion

    #pragma region LEAST COMMON MULTIPLE
        /* Calculates the smallest positive integer that is a multiple of two or more given numbers */
        template <std::integral T>
        [[nodiscard]] constexpr T LCM(T a, T b) noexcept
        {
            if (a == 0 || b == 0) return 0;
            T g = GCD(a, b);
            T absA = Abs(a);
            T absB = Abs(b);
            assert(absA / g <= std::numeric_limits<T>::max() / absB && "LCM: Integer overflow, (absA / g) * absB would exceed max value for type T");
            return (absA / g) * absB;
        }
    #pragma endregion

    #pragma region COMPARISON/RANGE
        /* Restricts a value to within a minimum and maximum range */
        template<typename T>
        [[nodiscard]] constexpr T Clamp(T val, T minVal, T maxVal) noexcept(noexcept(val < minVal) && noexcept(val > maxVal))
        {
            return val < minVal ? minVal : (val > maxVal ? maxVal : val);
        }

        /* Returns the lesser of two values */
        template<typename T, typename... Ts>
        [[nodiscard]] constexpr T Min(T a, Ts... args) noexcept((true && ... && noexcept(a < args)))
        {
            T result = a;
            ((result = (args < result ? args : result)), ...);
            return result;
        }

        /* Returns the greater of two values */
        template<typename T, typename... Ts>
        [[nodiscard]] constexpr T Max(T a, Ts... args) noexcept((true && ... && noexcept(a > args)))
        {
            T result = a;
            ((result = (args > result ? args : result)), ...);
            return result;
        }
    #pragma endregion

    #pragma region COMBINATORICS
        /* Returns n!, the product of the first n natural numbers */
        [[nodiscard]] constexpr unsigned long long Factorial(int n) noexcept
        {
            // 21! = 51,090,942,171,709,440,000 > ULLONG_MAX (approx. 1.84e19)
            assert(n >= 0 && n <= 20 && "Factorial: n >= 0! && n <= 20!");
            constexpr unsigned long long factorialTable[] = {
                1ULL,                               // 0!
                1ULL,                               // 1!
                2ULL,                               // 2!
                6ULL,                               // 3!
                24ULL,                              // 4!
                120ULL,                             // 5!
                720ULL,                             // 6!
                5040ULL,                            // 7!
                40320ULL,                           // 8!
                362'880ULL,                         // 9!
                3'628'800ULL,                       // 10!
                39'916'800ULL,                      // 11!
                479'001'600ULL,                     // 12!
                6'227'020'800ULL,                   // 13!
                87'178'291'200ULL,                  // 14!
                1'307'674'368'000ULL,               // 15!
                20'922'789'888'000ULL,              // 16!
                355'687'428'096'000ULL,             // 17!
                6'402'373'705'728'000ULL,           // 18!
                121'645'100'408'832'000ULL,         // 19!
                2'432'902'008'176'640'000ULL        // 20!
            };

            return factorialTable[n];
        }

        /* Computes the modular inverse of a modulo m using the extended Euclidean algorithm, i.e., finds x such that (a * x) % m == 1 */
        template<std::integral T>
            requires std::is_unsigned_v<T> || std::is_signed_v<T>
        [[nodiscard]] constexpr T ModInverse(T a, T m) noexcept
        {
            assert(m > 0 && "ModInverse: modulus must be positive");
            assert(GCD(a, m) == 1 && "ModInverse: inverse does not exist when GCD(a, m) != 1");
            if (m == 1) return 0;

            long long mod = static_cast<long long>(m);
            long long x0 = 0, x1 = 1;
            long long t, q;
            long long aSigned = static_cast<long long>(a);

            while (aSigned > 1)
            {
                q = aSigned / mod;
                t = mod;

                mod = aSigned % mod;
                aSigned = t;
                t = x0;

                x0 = x1 - q * x0;
                x1 = t;
            }
            if (x1 < 0) x1 += m;

            return static_cast<T>(x1);
        }

        /* Computes the binomial coefficient C(n, k) modulo p safely for n, k < p using factorials and the modular inverse of k! */
        [[nodiscard]] constexpr unsigned long long ModBinomialSmall(unsigned long long n, unsigned long long k, unsigned long long p) noexcept
        {
            assert(p > 1 && "ModBinomialSmall: modulus must be greater than 1");
            assert(n < p && k < p && "ModBinomialSmall: n and k must be less than p");
            if (k > n) return 0;
            if (k == 0 || k == n) return 1;

            if (k > n - k) k = n - k;
            unsigned long long numerator = 1;
            unsigned long long denominator = 1;

            for (unsigned long long i = 0; i < k; ++i)
            {
                numerator = (numerator * (n - i)) % p;
                denominator = (denominator * (i + 1)) % p;
            }

            return (numerator * ModInverse(denominator, p)) % p;
        }

        /* Computes C(n, k) % p for arbitrary n, k using Lucas' theorem recursively, reducing the problem to calls of ModBinomialSmall */
        [[nodiscard]] constexpr unsigned long long ModBinomial(unsigned long long n, unsigned long long k, unsigned long long p) noexcept
        {
            assert(p > 1 && "ModBinomial: modulus must be greater than 1 (preferably prime)");
            if (k > n) return 0;
            if (k == 0 || k == n) return 1;

            unsigned long long result = 1;

            while (n > 0 || k > 0)
            {
                const unsigned long long nModP = n % p;
                const unsigned long long kModP = k % p;
                if (kModP > nModP) return 0;

                result = (result * ModBinomialSmall(nModP, kModP, p)) % p;
                n /= p;
                k /= p;
            }

            return result;
        }

        #pragma region EXPONENTIATION BY SQUARING
            /* Computes the value of a base raised to a given power (integral) */
            template<typename T>
                requires std::integral<T>
            [[nodiscard]] constexpr T Pow(T base, int exponent) noexcept
            {
                if (exponent < 0) return 0;
                if (exponent == 0) return 1;

                T result = 1;
                while (exponent > 0)
                {
                    if (exponent & 1)
                    {
                        assert(result == 0 || base == 0 || Abs(result) <= std::numeric_limits<T>::max() / Abs(base)
                            && "Pow: Integer overflow, result * base would exceed max value for this type");
                        result *= base;
                    }
                    exponent >>= 1;
                    if (exponent)
                    {
                        assert(base == 0 || Abs(base) <= std::numeric_limits<T>::max() / Abs(base)
                            && "Pow: Integer overflow, base * base would exceed max value for this type");
                        base *= base;
                    }
                }
                return result;
            }
            /* Computes the value of a base raised to a given power (floating point) */
            template<typename T>
                requires std::floating_point<T>
            [[nodiscard]] constexpr T Pow(T base, int exponent) noexcept
            {
                if (exponent == 0)
                    return static_cast<T>(1);
                if (base == static_cast<T>(0))
                    return exponent > 0 ? static_cast<T>(0) : std::numeric_limits<T>::infinity();

                bool negExp = exponent < 0;
                int exp = negExp ? -exponent : exponent;
                T result = Exp(static_cast<T>(exp) * Log(base));
                return negExp ? static_cast<T>(1) / result : result;
            }
        #pragma endregion
    #pragma endregion

    #pragma region SEQUENCE/RECURRENCE
        /* Computes the n-th Fibonacci number */
        [[nodiscard]] constexpr unsigned long long Fibonacci(int n) noexcept
        {
            // Derived from Binet's formula: F(n) ~ (phi^n) / sqrt(5), where phi approx. 1.618
            // F(94) = 19,740,274,219,868,223,167 > ULLONG_MAX
            assert(n >= 0 && n <= 93 && "Fibonacci: n >= F(0) && n <= F(93)");
            if (n < 0 || n > 93) return 0;
            unsigned long long a = 0, b = 1;
            for (int i = 0; i < n; ++i)
            {
                a = Exchange(b, a + b);
            }
            return a;
        }

        /* Computes the sum of the first n natural numbers (int) */
        [[nodiscard]] constexpr int Sum(int n) noexcept
        {
            // n*(n+1)/2 <= INT_MAX -> n <= sqrt(2*INT_MAX) (approx. 46340)
            assert(n >= 0 && n <= 46340 && "Sum: n >= 0 && n <= 46340");
            if (n < 0 || n > 46340) return 0;
            return n > 0 ? n * (n + 1) / 2 : 0;
        }
        /* Computes the sum of the first n natural numbers (long long) */
        [[nodiscard]] constexpr long long Sum(long long n) noexcept
        {
            // n*(n+1)/2 <= LLONG_MAX -> n <= sqrt(2*LLONG_MAX)
            assert(n >= 0 && n <= 3037000499LL && "Sum: n >= 0 && n <= 3037000499LL");
            if (n < 0 || n > 3037000499LL) return 0;
            return n * (n + 1) / 2;
        }
        /* Computes the sum of the first n natural numbers (unsigned long long) */
        [[nodiscard]] constexpr unsigned long long Sum(unsigned long long n) noexcept
        {
            constexpr unsigned long long maxN = 0x1FFFFFFFFFFFFF;
            // n*(n+1)/2 <= ULLONG_MAX  -> n <= sqrt(2*ULLONG_MAX + 0.25) - 0.5
            assert(n <= maxN && "Sum: input too large, would overflow unsigned long long");
            return n <= maxN ? n * (n + 1) / 2 : 0;
        }

        /* Computes the sum of the squares of the first n natural numbers (int) */
        [[nodiscard]] constexpr int SumSquares(int n) noexcept
        {
            // n*(n+1)*(2n+1)/6 <= INT_MAX -> n approx. 181
            assert(n >= 0 && n <= 181 && "SumSquares: n >= 0 && n <= 181");
            if (n < 0 || n > 181) return 0;
            return n > 0 ? n * (n + 1) * (2 * n + 1) / 6 : 0;
        }
        /* Computes the sum of the squares of the first n natural numbers (long long) */
        [[nodiscard]] constexpr long long SumSquares(long long n) noexcept
        {
            // n*(n+1)*(2n+1)/6 <= LLONG_MAX -> n approx. 30303
            assert(n >= 0 && n <= 30303LL && "SumSquares: n >= 0 && n <= 30303LL");
            if (n < 0 || n > 30303LL) return 0;
            return n * (n + 1) * (2 * n + 1) / 6;
        }
        /* Computes the sum of the squares of the first n natural numbers (unsigned long long) */
        [[nodiscard]] constexpr unsigned long long SumSquares(unsigned long long n) noexcept
        {
            constexpr unsigned long long maxN = 2642245ULL;
            // n*(n+1)*(2n+1)/6 <= ULLONG_MAX -> solve for n
            assert(n <= maxN && "SumSquares: input too large, would overflow unsigned long long");
            return n <= maxN ? n * (n + 1) * (2 * n + 1) / 6 : 0;
        }
    #pragma endregion
    }; // end of namespace math_utils
}; // end of namespace winxframe

#endif