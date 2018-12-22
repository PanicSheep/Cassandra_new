#pragma once
#include <cassert>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <omp.h>
#include <ostream>
#include <vector>

template <typename T>
std::vector<T>& operator+=(std::vector<T>& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a[i] += b[i];
	return a;
}

template <typename T>
std::vector<T> operator+=(std::vector<T>&& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a[i] += b[i];
	return a;
}

template <typename T>
std::vector<T>& operator-=(std::vector<T>& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a[i] -= b[i];
	return a;
}

template <typename T>
std::vector<T> operator-=(std::vector<T>&& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a[i] -= b[i];
	return a;
}

template <typename T>
std::vector<T>& operator*=(std::vector<T>& a, const T& b)
{
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a[i] *= b;
	return a;
}

template <typename T>
std::vector<T>& operator/=(std::vector<T>& a, const T& b)
{
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a[i] /= b;
	return a;
}

template <typename T>
std::vector<T>& operator*=(std::vector<T>& a, const std::vector<T>& b)
{
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a[i] *= b[i];
	return a;
}

template <typename T>
std::vector<T> operator-(std::vector<T> a)
{
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a[i] = -a[i];
	return a;
}

template <typename T>
std::vector<T> operator/(std::vector<T> a, const std::vector<T>& b)
{
	assert(a.size() == b.size());
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a[i] /= b[i];
	return a;
}

template <typename T> std::vector<T> operator+(      std::vector<T>  a, const std::vector<T>&  b) { return a += b; }
template <typename T> std::vector<T> operator+(const std::vector<T>& a,       std::vector<T>&& b) { return b += a; }

template <typename T> std::vector<T> operator-(      std::vector<T>  a, const std::vector<T>&  b) { return a -= b; }
template <typename T> std::vector<T> operator-(const std::vector<T>& a,       std::vector<T>&& b) { return -b += a; }

template <typename T> std::vector<T> operator*(std::vector<T> a, const T& b) { return a *= b; }
template <typename T> std::vector<T> operator*(const T& b, std::vector<T> a) { return a *= b; }

template <typename T> std::vector<T> operator*(      std::vector<T>  a, const std::vector<T>&  b) { return a *= b; }
template <typename T> std::vector<T> operator*(const std::vector<T>& a,       std::vector<T>&& b) { return b *= a; }

template <typename T> std::vector<T> operator/(std::vector<T> a, const T& b) { return a /= b; }

template <typename T>
bool operator==(const std::vector<T>& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());
	const int64_t size = a.size();
	for (int64_t i = 0; i < size; i++)
		if (a[i] != b[i])
			return false;
	return true;
}

template <typename T> bool operator!=(const std::vector<T>& a, const std::vector<T>& b) { return !(a == b); }

template <typename T>
T dot(const std::vector<T>& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());
	const int64_t size = a.size();
	long double sum = 0;
	#pragma omp parallel for reduction(+ : sum)
	for (int64_t i = 0; i < size; i++)
		sum += a[i] * b[i];
	return static_cast<T>(sum);
}

template <typename T> T norm(const std::vector<T>& a) { return sqrt(dot(a, a)); }

template <typename T> std::vector<T> sqrt(std::vector<T> a)
{
	const int64_t size = a.size();
	#pragma omp parallel for
	for(int64_t i = 0; i < size; i++)
		a[i] = sqrt(a[i]);
	return a;
}

template <typename T> std::vector<T> inv(std::vector<T> a)
{
	const int64_t size = a.size();
	#pragma omp parallel for
	for(int64_t i = 0; i < size; i++)
		a[i] = (a[i] == 0) ? static_cast<T>(1) : static_cast<T>(1) / a[i];
	return a;
}

template <typename T>
void Decompose(T& length, std::vector<T>& unitvector, const std::vector<T>& inputVector)
{
	length = norm(inputVector);
	unitvector = static_cast<T>(1 / length) * inputVector;
}

namespace std
{
	template <typename T>
	std::string to_string(const std::vector<T>& vec)
	{
		std::string s = "(";
		for (std::size_t i = 0; i < vec.size() - 1; i++)
			s += std::to_string(vec[i]) + ", ";
		if (!vec.empty())
			s += vec.back();
		s += ")";
		return s;
	}
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
	return os << to_string(vec);
}