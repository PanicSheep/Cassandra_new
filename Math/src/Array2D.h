#pragma once
#include <vector>
#include <cstdint>

template <typename T>
class Array2D
{
	std::size_t m_n; // Number of rows.
	std::size_t m_m; // Number of columns.
	std::vector<T> m_arr;
public:
	Array2D(std::size_t n, std::size_t m) : m_n(n), m_m(m) { m_arr.resize(n * m); }
	Array2D(               ) = default;
	Array2D(const Array2D& ) = default;
	Array2D(      Array2D&&) = default;

	Array2D& operator=(const Array2D& ) = default;
	Array2D& operator=(      Array2D&&) = default;

	std::size_t n() const noexcept { return m_n; } // Number of rows.
	std::size_t m() const noexcept { return m_m; } // Number of columns.

	const T& operator()(std::size_t i, std::size_t j) const { return m_arr[i * m_m + j]; }
	      T& operator()(std::size_t i, std::size_t j)       { return m_arr[i * m_m + j]; }
};
