#pragma once
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <math.h>
#include <omp.h>
#include <ostream>
#include <string>
#include <type_traits>
#include <vector>
#include "Path.h"

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

template <typename T>
std::vector<T> read_vector(const std::string & filename, std::size_t size = 0xFFFFFFFFFFFFFFFFULL)
{
	std::fstream file(filename, std::ios::in | std::ios::binary);
	if (!file.is_open())
		throw std::iostream::failure("File '" + filename + "' could not be opened for input.");

	std::vector<T> vec;
	T buffer;
	while ((size > 0) && file.read(reinterpret_cast<char*>(&buffer), sizeof(T))) {
		vec.push_back(buffer);
		size--;
	}

	file.close();
	return vec;
}

template <typename T>
std::vector<T> read_vector(const CPath & file, std::size_t size = 0xFFFFFFFFFFFFFFFFULL)
{
	return read_vector<T>(file.GetAbsoluteFilePath(), size);
}

template <typename T>
void write_to_file(const std::string & filename, const std::vector<T>& vec)
{
	std::fstream file(filename, std::ios::out | std::ios::binary);
	if (!file.is_open())
		throw std::iostream::failure("File '" + filename + "' could not be opened for output.");

	file.write(reinterpret_cast<const char*>(&vec[0]), sizeof(T) * vec.size());

	file.close();
}

template <typename T>
void write_to_file(const CPath & file, const std::vector<T>& vec)
{
	write_to_file(file.GetAbsoluteFilePath(), vec);
}

template <typename Iterator>
void write_to_file(const std::string & filename, Iterator begin, Iterator end)
{
	std::fstream file(filename, std::ios::out | std::ios::binary);
	if (!file.is_open())
		throw std::iostream::failure("File '" + filename + "' could not be opened.");

	for (auto it = begin; it != end; ++it)
		file.write(reinterpret_cast<const char*>(&*it), sizeof(typename Iterator::value_type));

	file.close();
}

template <typename Iterator>
void write_to_file(const CPath & file, Iterator begin, Iterator end)
{
	write_to_file(file.GetAbsoluteFilePath(), begin, end);
}