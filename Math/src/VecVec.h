#pragma once
#include <vector>
#include <cstdint>
#include <cmath>
#include "VectorExtension.h"

template <typename T>
class Vecvec
{
	std::size_t m_n = 0;
	std::vector<std::vector<T>> m_vec;
public:
	Vecvec(std::size_t n) : m_n(n) { m_vec.resize(n); }
	Vecvec(std::vector<std::size_t> ns);
	Vecvec(std::vector<std::size_t> ns, T value);
	Vecvec() = default;
	Vecvec(const Vecvec& ) = default;
	Vecvec(      Vecvec&&) = default;

	Vecvec& operator=(const Vecvec& ) = default;
	Vecvec& operator=(      Vecvec&&) = default;

	inline             std::size_t  size() const { return m_n; }
	inline             std::size_t  n()    const { return m_n; }
	inline std::vector<std::size_t> ns()   const;

	inline const std::vector<T>& operator()(std::size_t i) const { return m_vec[i]; }
	inline       std::vector<T>& operator()(std::size_t i)       { return m_vec[i]; }

	inline Vecvec<T>& sqrt();
	inline Vecvec<T>& inv();
	inline long double StandardDeviation() const;
};

template <typename T>
Vecvec<T>::Vecvec(std::vector<std::size_t> ns)
	: m_n(ns.size())
{
	m_vec.reserve(m_n);
	for (const auto& it : ns)
		m_vec.push_back(std::vector<T>(it));
}

template <typename T>
Vecvec<T>::Vecvec(std::vector<std::size_t> ns, T value)
	: m_n(ns.size())
{
	m_vec.reserve(m_n);
	for (const auto& it : ns)
		m_vec.push_back(std::vector<T>(it, value));
}

template <typename T>
std::vector<std::size_t> Vecvec<T>::ns() const
{
	std::vector<std::size_t> ret;
	ret.reserve(n());
	for (const auto& it : m_vec)
		ret.push_back(it.size());
	return ret;
}

template <typename T>
Vecvec<T>& operator+=(Vecvec<T>& a, const Vecvec<T>& b)
{
	assert(a.size() == b.size());
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a(i) += b(i);
	return a;
}

template <typename T>
Vecvec<T> operator+=(Vecvec<T>&& a, const Vecvec<T>& b)
{
	assert(a.size() == b.size());
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a(i) += b(i);
	return a;
}

template <typename T>
Vecvec<T>& operator-=(Vecvec<T>& a, const Vecvec<T>& b)
{
	assert(a.size() == b.size());
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a(i) -= b(i);
	return a;
}

template <typename T>
Vecvec<T> operator-=(Vecvec<T>&& a, const Vecvec<T>& b)
{
	assert(a.size() == b.size());
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a(i) -= b(i);
	return a;
}

template <typename T>
Vecvec<T>& operator*=(Vecvec<T>& a, const T& b)
{
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a(i) *= b;
	return a;
}

template <typename T>
Vecvec<T>& operator/=(Vecvec<T>& a, const T& b)
{
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a(i) /= b;
	return a;
}

template <typename T>
Vecvec<T>& operator*=(Vecvec<T>& a, const Vecvec<T>& b)
{
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a(i) *= b(i);
	return a;
}

template <typename T>
Vecvec<T> operator-(Vecvec<T> a)
{
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a(i) = -a(i);
	return a;
}

template <typename T>
Vecvec<T> operator/(Vecvec<T> a, const Vecvec<T>& b)
{
	assert(a.size() == b.size());
	const int64_t size = a.size();
	#pragma omp parallel for
	for (int64_t i = 0; i < size; i++)
		a(i) = a(i) / b(i);
	return a;
}

template <typename T> inline Vecvec<T> operator+(      Vecvec<T>  a, const Vecvec<T>&  b) { return a += b; }
template <typename T> inline Vecvec<T> operator+(const Vecvec<T>& a,       Vecvec<T>&& b) { return b += a; }

template <typename T> inline Vecvec<T> operator-(      Vecvec<T>  a, const Vecvec<T>&  b) { return a -= b; }
template <typename T> inline Vecvec<T> operator-(const Vecvec<T>& a,       Vecvec<T>&& b) { return -b += a; }

template <typename T> inline Vecvec<T> operator*(Vecvec<T> a, const T& b) { return a *= b; }
template <typename T> inline Vecvec<T> operator*(const T& b, Vecvec<T> a) { return a *= b; }

template <typename T> Vecvec<T> operator*(      Vecvec<T>  a, const Vecvec<T>&  b) { return a *= b; }
template <typename T> Vecvec<T> operator*(const Vecvec<T>& a,       Vecvec<T>&& b) { return b *= a; }

template <typename T>
T dot(const Vecvec<T>& a, const Vecvec<T>& b)
{
	assert(a.size() == b.size());
	const int64_t size = a.size();
	long double sum = 0;
	#pragma omp parallel for reduction(+ : sum)
	for (int64_t i = 0; i < size; i++)
		sum += dot(a(i), b(i));
	return static_cast<T>(sum);
}

template <typename T> inline T norm(const Vecvec<T>& a) { return sqrt(dot(a, a)); }

template <typename T> inline Vecvec<T>& Vecvec<T>::sqrt()
{
	for (auto& it : m_vec)
		it = ::sqrt(it);
	return *this;
}

template <typename T> inline Vecvec<T> sqrt(Vecvec<T> a)
{
	return a.sqrt();
}

template <typename T> inline Vecvec<T>& Vecvec<T>::inv()
{
	for (auto& it : m_vec)
		it = ::inv(it);
	return *this;
}

template <typename T> inline Vecvec<T> inv(Vecvec<T> a)
{
	return a.inv();
}

template <typename T>
inline void Decompose(T& length, Vecvec<T>& unitvector, const Vecvec<T>& inputVector)
{
	length = norm(inputVector);
	unitvector = static_cast<T>(1.0 / length) * inputVector;
}

template <typename T>
long double Vecvec<T>::StandardDeviation() const
{
	long double E_of_X = 0;
	long double E_of_X_sq = 0;
	std::size_t N = 0;

	for (const auto& it : m_vec)
		for (const auto& elem : it) {
			N++;
			E_of_X += (static_cast<long double>(elem) - E_of_X) / static_cast<long double>(N);
			E_of_X_sq += (static_cast<long double>(elem)*static_cast<long double>(elem) - E_of_X_sq) / static_cast<long double>(N);
		}

	return std::sqrt(E_of_X_sq - E_of_X * E_of_X);
}


template <typename T>
long double StandardDeviation(const Vecvec<T>& vec)
{
	return vec.StandardDeviation();
}
