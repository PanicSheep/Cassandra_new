#pragma once
#include <cassert>
#include <iterator>
#include <math.h>
#include <vector>
#include <omp.h>

template <typename T>
std::vector<T>& operator+=(std::vector<T>& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());
	#pragma omp parallel for
	for (long long i = 0; i < static_cast<long long>(a.size()); i++) a[i] += b[i];
	return a;
}


template <typename T>
std::vector<T>& operator-=(std::vector<T>& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());
	#pragma omp parallel for
	for (long long i = 0; i < static_cast<long long>(a.size()); i++) a[i] -= b[i];
	return a;
}

template <typename T>
std::vector<T> operator*=(std::vector<T>& a, const T& b)
{
	#pragma omp parallel for
	for (long long i = 0; i < static_cast<long long>(a.size()); i++) a[i] *= b;
	return a;
}

template <typename T>
std::vector<T> operator/=(std::vector<T>& a, const T& b)
{
	#pragma omp parallel for
	for (long long i = 0; i < static_cast<long long>(a.size()); i++) a[i] /= b;
	return a;
}

template <typename T>
std::vector<T> operator*=(std::vector<T>& a, const std::vector<T>& b)
{
	#pragma omp parallel for
	for (long long i = 0; i < static_cast<long long>(a.size()); i++) a[i] *= b[i];
	return a;
}

template <typename T>
std::vector<T> operator-(std::vector<T> a)
{
	#pragma omp parallel for
	for (long long i = 0; i < static_cast<long long>(a.size()); i++) a[i] = -a[i];
	return a;
}

template <typename T> inline std::vector<T> operator+(      std::vector<T>  a, const std::vector<T>&  b) { return a += b; }
template <typename T> inline std::vector<T> operator+(const std::vector<T>& a,       std::vector<T>&& b) { return b += a; }

template <typename T> inline std::vector<T> operator-(      std::vector<T>  a, const std::vector<T>&  b) { return a -= b; }
template <typename T> inline std::vector<T> operator-(const std::vector<T>& a,       std::vector<T>&& b) { return -std::move(b) += a; }

template <typename T> inline std::vector<T> operator*(std::vector<T> a, const T& b) { return a *= b; }
template <typename T> inline std::vector<T> operator*(const T& b, std::vector<T> a) { return a *= b; }

template <typename T> std::vector<T> operator*(      std::vector<T>  a, const std::vector<T>&  b) { return a *= b; }
template <typename T> std::vector<T> operator*(const std::vector<T>& a,       std::vector<T>&& b) { return b *= a; }

template <typename T> std::vector<T> operator/(std::vector<T> a, const T& b) { return a /= b; }

template <typename T>
bool operator==(const std::vector<T>& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());
	for (long long i = 0; i < a.size(); i++)
		if (a[i] != b[i]) return false;
	return true;
}

template <typename T> inline bool operator!=(const std::vector<T>& a, const std::vector<T>& b) { return !(a == b); }

template <typename T>
T dot(const std::vector<T>& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());
	long double sum = 0;
	#pragma omp parallel for reduction(+ : sum)
	for (long long i = 0; i < static_cast<long long>(a.size()); i++) sum += a[i] * b[i];
	return static_cast<T>(sum);
}

template <typename T> inline T norm(const std::vector<T>& a) { return sqrt(dot(a, a)); }

template <typename T> inline std::vector<T> sqrt(std::vector<T> a)
{
	#pragma omp parallel for
	for(long long i = 0; i < static_cast<long long>(a.size()); i++)
		a[i] = sqrt(a[i]);
	return a;
}

template <typename T>
inline void Decompose(T& length, std::vector<T>& unitvector, const std::vector<T>& inputVector)
{
	length = norm(inputVector);
	unitvector = static_cast<T>(1 / length) * inputVector;
}