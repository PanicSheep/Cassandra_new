#pragma once
#include "MacrosHell.h"
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

inline int RoundInt(const double d) { return static_cast<int>(std::round(d)); }
inline int RoundInt(const float  f) { return static_cast<int>(std::round(f)); }

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
std::string SignedInt(int score);

// Returns a double digit int with either a '+' or a '-' prefix. Zero is represented '+00'.
 std::string DoubleDigitSignedInt(int score);

 std::size_t ParseBytes(const std::string& bytes);

uint64_t FlipCodiagonal(uint64_t);
uint64_t FlipDiagonal(uint64_t);
uint64_t FlipHorizontal(uint64_t);
uint64_t FlipVertical(uint64_t);