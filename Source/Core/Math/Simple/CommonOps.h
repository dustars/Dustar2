/*
	Description:
	Common math operations

	1. Use constexpr to store data?
*/


#pragma once
#include <cstddef>
#include <math.h>
#include <algorithm>

//It's better to use define?
static const float		PI = 3.14159265358979323846f;

//It's pi...divided by 360.0f!
static const float		PI_OVER_360 = PI / 360.0f;

//Radians to degrees
static inline double RadToDeg(const double deg)
{
	return deg * 180.0 / PI;
};

//Degrees to radians
static inline double DegToRad(const double rad)
{
	return rad * PI / 180.0;
};

//Just an int type, thus it won't be THAT large. Keep this in mind when x is big.
static inline int Factorial(int x) {
	if (!x) return 1;
	for (std::size_t i = x - 1; i > 0; i--) x *= i;
	return x;
}

static inline float EulerDistance(float x, float y, float z, float x1, float y1, float z1) {
	float disX = x1 - x;
	float disY = y1 - y;
	float disZ = z1 - z;
	disX *= disX;
	disY *= disY;
	disZ *= disZ;
	disX += disY + disZ;
	return sqrt(disX);
}

static inline float Remap(float value, float oldMin, float oldMax, float newMin, float newMax)
{
	return newMin + (((value - oldMin) / (oldMax - oldMin)) * (newMax - newMin));
}

static inline float Clamp(float value, float min, float max)
{
	if (value > max) return max;
	if (value < min) return min;
	return value;
}