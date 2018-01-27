#pragma once
#include "macros_hell.h"
#include <cassert>
#include <chrono>
#include <cmath>
#include <ctime>
#include <cstdint>
#include <ratio>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

#ifdef _MSC_VER
	#include <direct.h>
#else
	#include <unistd.h>
#endif

// TODO: Remove!
template <typename S, typename T, typename U> inline S CLAMP(const S & value, const T & min, const U & max) { return value < min ? min : (value > max ? max : value); }

inline int RoundInt(const double d) { return static_cast<int>(std::round(d)); }
inline int RoundInt(const float  f) { return static_cast<int>(std::round(f)); }
//inline int RoundInt(double d) { int i = static_cast<int>(d); return i + (d - i >= .5 ) - (d - i <= -.5 ); } // TODO: Test if this is faster.
//inline int RoundInt(float  f) { int i = static_cast<int>(f); return i + (f - i >= .5f) - (f - i <= -.5f); } // TODO: Test if this is faster.

std::string time_format(const std::chrono::milliseconds duration);

template <class U, class V>
std::string time_format(const std::chrono::duration<U, V> duration)
{
	return time_format(std::chrono::duration_cast<std::chrono::milliseconds>(duration));
}

std::string short_time_format(std::chrono::duration<long long, std::pico> duration);
std::string ThousandsSeparator(uint64_t n);
std::string DateTimeNow();

void replace_all(std::string& source, const std::string& find, const std::string& replace);

std::string GetCurrentWorkingDirectory();

std::vector<std::string> split(const std::string& src, const std::string& deli);
std::string join(const std::vector<std::string>& parts, const std::string& deli);

// Returns an int with either a '+' or a '-' prefix. Zero is represented '+0'.
inline std::string SignedInt(int score)
{
	const std::string Sign = (score >= 0) ? "+" : "-";
	const std::string Number = std::to_string(std::abs(score));
	return Sign + Number;
}

// Returns a double digit int with either a '+' or a '-' prefix. Zero is represented '+00'.
inline std::string DoubleDigitSignedInt(int score)
{
	const std::string Sign = (score >= 0) ? "+" : "-";
	const std::string FillingZero = (std::abs(score) < 10) ? "0" : "";
	const std::string Number = std::to_string(std::abs(score));
	return Sign + FillingZero + Number;
}
