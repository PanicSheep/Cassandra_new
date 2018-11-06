#pragma once
#include <algorithm>
#include <cassert>
#include <numeric>
#include <omp.h>
#include <vector>
#include <cstdint>
#include <iostream>
#include <fstream>
#include "VectorExtension.h"

/// Compressed Sparse Row Matrix
template <typename ValueType, typename SizeType = std::size_t>
class CMatrix_CSR
{
	std::size_t m_n; ///< number of rows
	std::size_t m_m; ///< number of columns

	std::vector<ValueType> data;
	std::vector<SizeType> col_indices;
	std::vector<SizeType> row_starts;

public:
	CMatrix_CSR(const std::size_t m = 0);
	CMatrix_CSR(const std::string & filename);
	CMatrix_CSR(const CMatrix_CSR&) = default;
	CMatrix_CSR(CMatrix_CSR&&) = default;
	CMatrix_CSR& operator=(const CMatrix_CSR&);
	CMatrix_CSR& operator=(CMatrix_CSR&&) = default;

	void load(const std::string & filename);
	void save(const std::string & filename) const;

	inline std::size_t n() const;
	inline std::size_t m() const;
	inline std::size_t nnz() const;
	inline std::size_t size() const;

	void push_back(const std::vector<SizeType>& Col_Indices, const std::vector<ValueType>& Data);
	void push_back(const SizeType& col, const ValueType& Data);
	inline void endRow();

	template <typename T> std::vector<T> Ax(const std::vector<T>& x) const;
	template <typename T> std::vector<T> ATx(const std::vector<T>& x) const;
	template <typename T> std::vector<T> ATAx(const std::vector<T>& x) const;
	template <typename T> std::vector<T> operator*(const std::vector<T>& x) const;

	inline bool operator==(const CMatrix_CSR& mat) const;
	inline bool operator!=(const CMatrix_CSR& mat) const;

	/// Inverse Jacobi Preconditioner Square
	/// Returns diag(A' * A)
	template <typename T> std::vector<T> DiagATA() const;

	/// Jacobi Preconditioner Square
	/// Returns 1 / diag(A' * A)
	template <typename T> std::vector<T> JacobiPreconditionerSquare() const;
};

// ################################################################################################
// Inline section
// ################################################################################################
// ------------------------------------------------------------------------------------------------
template <typename ValueType, typename SizeType>
CMatrix_CSR<ValueType, SizeType>::CMatrix_CSR(const std::size_t m)
	: m_n(0), m_m(m), data(0), col_indices(0), row_starts(1, 0)
{
}

template <typename ValueType, typename SizeType>
CMatrix_CSR<ValueType, SizeType>::CMatrix_CSR(const std::string & filename)
{
	load(filename);
}

template <typename ValueType, typename SizeType>
CMatrix_CSR<ValueType, SizeType>& CMatrix_CSR<ValueType, SizeType>::operator=(const CMatrix_CSR<ValueType, SizeType>& o)
{
	m_n = o.m_n;
	m_m = o.m_m;
	data = o.data;
	col_indices = o.col_indices;
	row_starts = o.row_starts;
	return *this;
}

template <typename ValueType, typename SizeType>
void CMatrix_CSR<ValueType, SizeType>::load(const std::string & filename)
{
	std::fstream file(filename, std::ios::in | std::ios::binary);
	if (!file.is_open())
		throw std::iostream::failure("File '" + filename + "' could not be opened for input.");

	std::size_t vec1size;
	std::size_t vec2size;

	file.read(reinterpret_cast<char*>(&m_n), sizeof(std::size_t));
	file.read(reinterpret_cast<char*>(&m_m), sizeof(std::size_t));
	file.read(reinterpret_cast<char*>(&vec1size), sizeof(std::size_t));
	file.read(reinterpret_cast<char*>(&vec2size), sizeof(std::size_t));

	data = std::vector<ValueType>(vec1size);
	col_indices = std::vector<SizeType >(vec1size);
	row_starts = std::vector<SizeType >(vec2size);

	file.read(reinterpret_cast<char*>(&data[0])       , sizeof(ValueType) * vec1size);
	file.read(reinterpret_cast<char*>(&col_indices[0]), sizeof(SizeType ) * vec1size);
	file.read(reinterpret_cast<char*>(&row_starts[0]) , sizeof(SizeType ) * vec2size);

	file.close();
}

template <typename ValueType, typename SizeType>
void CMatrix_CSR<ValueType, SizeType>::save(const std::string & filename) const
{
	std::fstream file(filename, std::ios::out | std::ios::binary);
	if (!file.is_open())
		throw std::iostream::failure("File '" + filename + "' could not be opened for output.");

	const std::size_t vec1size = data.size();
	const std::size_t vec2size = row_starts.size();

	file.write(reinterpret_cast<const char*>(&m_n)           , sizeof(std::size_t));
	file.write(reinterpret_cast<const char*>(&m_m)           , sizeof(std::size_t));
	file.write(reinterpret_cast<const char*>(&vec1size)      , sizeof(std::size_t));
	file.write(reinterpret_cast<const char*>(&vec2size)      , sizeof(std::size_t));
	file.write(reinterpret_cast<const char*>(&data[0])       , sizeof(ValueType) * vec1size);
	file.write(reinterpret_cast<const char*>(&col_indices[0]), sizeof(SizeType ) * vec1size);
	file.write(reinterpret_cast<const char*>(&row_starts[0]) , sizeof(SizeType ) * vec2size);

	file.close();
}

template <typename ValueType, typename SizeType>
inline std::size_t CMatrix_CSR<ValueType, SizeType>::n() const
{
	return m_n;
}

template <typename ValueType, typename SizeType>
inline std::size_t CMatrix_CSR<ValueType, SizeType>::m() const
{
	return m_m;
}

template <typename ValueType, typename SizeType>
inline std::size_t CMatrix_CSR<ValueType, SizeType>::nnz() const
{
	return std::count_if(data.begin(), data.end(), [](const ValueType& d) { return d != 0; });
}

template <typename ValueType, typename SizeType>
inline std::size_t CMatrix_CSR<ValueType, SizeType>::size() const
{
	return col_indices.size();
}

template <typename ValueType, typename SizeType>
void CMatrix_CSR<ValueType, SizeType>::push_back(const std::vector<SizeType>& Col_Indices, const std::vector<ValueType>& Data)
{
	for (auto col_index : Col_Indices)
		assert(col_index < m_m);

	data.insert(data.end(), Data.begin(), Data.end());
	col_indices.insert(col_indices.end(), Col_Indices.begin(), Col_Indices.end());
	endRow();
}

template <typename ValueType, typename SizeType>
void CMatrix_CSR<ValueType, SizeType>::push_back(const SizeType& col, const ValueType& Data)
{
	assert(col < m_m);
	data.push_back(Data);
	col_indices.push_back(col);
}

template <typename ValueType, typename SizeType>
inline void CMatrix_CSR<ValueType, SizeType>::endRow()
{
	row_starts.push_back(static_cast<SizeType>(col_indices.size()));
	++m_n;
}

template <typename ValueType, typename SizeType>
template <typename T>
std::vector<T> CMatrix_CSR<ValueType, SizeType>::Ax(const std::vector<T>& x) const
{
	assert(m_m == x.size());

	std::vector<T> result(m_n);
	#pragma omp parallel for
	for (long long i = 0; i < static_cast<long long>(m_n); i++)
	{
		T sum = 0; // prevent cache thrashing and better instruction level parallelism
		for (SizeType j = row_starts[i]; j < row_starts[i + 1]; j++)
			sum += static_cast<T>(data[j]) * x[col_indices[j]];
		result[i] = sum;
	}
	return result;
}

template <typename ValueType, typename SizeType>
template <typename T>
std::vector<T> CMatrix_CSR<ValueType, SizeType>::ATx(const std::vector<T>& x) const
{
	assert(m_n == x.size());

	std::vector<T> result(m_m, 0);
	#pragma omp parallel
	{
		std::vector<T> local_result(m_m, 0);

		#pragma omp for nowait
		for (long long i = 0; i < static_cast<long long>(m_n); i++)
		{
			for (SizeType j = row_starts[i]; j < row_starts[i + 1]; j++)
				local_result[col_indices[j]] += static_cast<T>(data[j]) * x[i];
		}

		#pragma omp critical
		{
			for (std::size_t i = 0; i < m_m; i++)
				result[i] += local_result[i];
		}
	}
	return result;
}

template <typename ValueType, typename SizeType>
template <typename T>
std::vector<T> CMatrix_CSR<ValueType, SizeType>::ATAx(const std::vector<T>& x) const
{
	return ATx(Ax(x)); // Room for optimization: Remove this!

	assert(m_m == x.size());
	std::vector<T> result(m_m, 0);
	#pragma omp parallel
	{
		T tmp;
		std::vector<T> local_result(m_m, 0);

		#pragma omp for nowait
		for (long long i = 0; i < static_cast<long long>(m_n); i++)
		{
			tmp = 0;
			for (SizeType j = row_starts[i]; j < row_starts[i + 1]; j++)
				tmp += static_cast<T>(data[j]) * x[col_indices[j]];
			for (SizeType j = row_starts[i]; j < row_starts[i + 1]; j++)
				local_result[col_indices[j]] += static_cast<T>(data[j]) * tmp;
		}

		#pragma omp critical
		{
			for (long long i = 0; i < static_cast<long long>(m_m); i++)
				result[i] += local_result[i];
		}
	}
	return result;
}

template <typename ValueType, typename SizeType>
template <typename T>
std::vector<T> CMatrix_CSR<ValueType, SizeType>::operator*(const std::vector<T>& x) const
{
	return Ax(x);
}

template <typename ValueType, typename SizeType>
inline bool CMatrix_CSR<ValueType, SizeType>::operator==(const CMatrix_CSR<ValueType, SizeType>& mat) const
{
	return (data == mat.data) && (col_indices == mat.col_indices) && (row_starts == mat.row_starts);
}

template <typename ValueType, typename SizeType>
inline bool CMatrix_CSR<ValueType, SizeType>::operator!=(const CMatrix_CSR<ValueType, SizeType>& mat) const
{
	return !this->operator==(mat);
}

template <typename ValueType, typename SizeType>
template <typename T>
std::vector<T> CMatrix_CSR<ValueType, SizeType>::DiagATA() const
{
	std::vector<T> ret(m_m, 0);
	#pragma omp parallel
	{
		std::vector<T> local_ret(m_m, 0);

		#pragma omp for nowait
		for (long long i = 0; i < static_cast<long long>(size()); i++)
			local_ret[col_indices[i]] += static_cast<T>(data[i]) * static_cast<T>(data[i]);

		#pragma omp critical
		{
			ret += local_ret;
		}
	}
	return ret;
}

template <typename ValueType, typename SizeType>
template <typename T>
std::vector<T> CMatrix_CSR<ValueType, SizeType>::JacobiPreconditionerSquare() const
{
	return inv(DiagATA<T>());
}
// ------------------------------------------------------------------------------------------------
// ################################################################################################
