#ifndef OPLO_CONSTANTS_H
#define OPLO_CONSTANTS_H

#include <functional>

namespace oplo
{

template <class T>
inline void HashCombine(std::size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<typename T>
struct Constants
{};

template<>
struct Constants<float>
{
	inline static float pi()
	{
		return 3.14159265358979323846f;
	}

	inline static float halfPi()
	{
		return 3.14159265358979323846f * 0.5f;
	}

	inline static float degToRad(float degrees)
	{
		return (pi() / 180.f * degrees);
	}

	inline static float radToDeg(float radians)
	{
		return 180.f / pi() * radians;
	}

	inline static double normalizeRadians(float angle)
	{
		return angle - (int)(angle / (pi() * 2.f)) * (pi() * 2.f);
	}

	inline static double normalizeDegrees(float  angle)
	{
		return angle - (int)(angle / (360)) * (360);
	}
};


template<>
struct Constants<double>
{
	inline static double pi()
	{
		return 3.14159265358979323846;
	}

	inline static double halfPi()
	{
		return 3.14159265358979323846 * 0.5;
	}

	inline static double degToRad(double degrees)
	{
		return (pi() / 180. * degrees);
	}

	inline static double radToDeg(double radians)
	{
		return 180. / pi() * radians;
	}

	inline static double normalizeRadians(double angle)
	{
		return angle - (int)(angle / (pi() * 2.)) * (pi() * 2.);
	}

	inline static double normalizeDegrees(double angle)
	{
		return angle - (int)(angle / (360)) * (360);
	}
};

}

#endif