#pragma once
#include <vector>
#include <cstdint>

template <typename T>
class Array2D
{
	std::size_t m_n; ///< Number of rows.
	std::size_t m_m; ///< Number of columns.
	std::vector<T> m_arr;
public:
	Array2D(std::size_t n, std::size_t m) : m_n(n), m_m(m) { m_arr.resize(n*m); }
	Array2D(                 ) = default;
	Array2D(const Array2D&  o) = default;
	Array2D(      Array2D&& o) = default;

	Array2D& operator=(const Array2D&  o) = default;
	Array2D& operator=(      Array2D&& o) = default;
	
	inline std::size_t n() const { return m_n; } ///< Number of rows.
	inline std::size_t m() const { return m_m; } ///< Number of columns.
	
	inline const T& operator()(std::size_t i, std::size_t j) const { return m_arr[i*m_m+j]; }
	inline       T& operator()(std::size_t i, std::size_t j)       { return m_arr[i*m_m+j]; }
};
