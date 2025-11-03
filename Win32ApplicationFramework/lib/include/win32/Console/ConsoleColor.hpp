/*!
lib\include\win32\Console\ColorAttribute.hpp
Created: November 1, 2025
Updated: November 1, 2025
Copyright (c) 2025, Jacob Gosse

Color Attributes header file.
*/

#pragma once

#ifndef COLORATTRIBUTE_HPP_
#define COLORATTRIBUTE_HPP_

#include <cstdint>

struct ColorAttribute
{
private:
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;

public:
	constexpr ColorAttribute(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255)
		: red(r), green(g), blue(b), alpha(a) {}
};

constexpr ColorAttribute Red(255, 0, 0);

#endif