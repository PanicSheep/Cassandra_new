#pragma once
#include <vector>

template <typename T>
long double StandardDeviation(const std::vector<T>& vec)
{
	long double E_of_X = 0;
	long double E_of_X_sq = 0;
	for (std::size_t i = 0; i < vec.size(); i++)
	{
		const auto x = static_cast<long double>(vec[i]);
		const auto N = static_cast<long double>(i + 1);
		E_of_X += (x - E_of_X) / N;
		E_of_X_sq += (x * x - E_of_X_sq) / N;
	}
	return std::sqrtl(E_of_X_sq - E_of_X * E_of_X);
}