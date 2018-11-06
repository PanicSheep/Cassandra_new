#pragma once
#include <algorithm>
#include <cassert>
#include <numeric>
#include <omp.h>
#include <vector>
#include <cstdint>
#include "matrixCSR.h"
#include "vecvec.h"
#include "array2D.h"

/// Compressed Sparse Row Matrix Grid
template <typename ValueType, typename SizeType = std::size_t>
class CMatrix_CSR_Grid
{
public:
	typedef CMatrix_CSR<ValueType, SizeType> Matrix;
private:
	Array2D<Matrix> m_arr;
	bool check_dimensions() const;
public:
	CMatrix_CSR_Grid() = default;
	CMatrix_CSR_Grid(Array2D<Matrix> arr);
	CMatrix_CSR_Grid(const Array2D<std::string>& filenames);

	void load(const Array2D<std::string>& filenames);
	void save(const Array2D<std::string>& filenames) const;

	inline std::size_t n() const { return m_arr.n(); }
	inline std::size_t m() const { return m_arr.m(); }
	inline std::size_t nnz() const; ///< Numer of nonzero matrix elements
	inline std::size_t size() const; ///< Number of matrix elements
	inline std::vector<std::size_t> ns() const;
	inline std::vector<std::size_t> ms() const;

	template <typename T> Vecvec<T> Ax(const Vecvec<T>& x) const;
	template <typename T> Vecvec<T> ATx(const Vecvec<T>& x) const;
	template <typename T> Vecvec<T> ATAx(const Vecvec<T>& x) const;
	template <typename T> Vecvec<T> operator*(const Vecvec<T>& x) const;

	inline bool operator==(const CMatrix_CSR_Grid& mat) const;
	inline bool operator!=(const CMatrix_CSR_Grid& mat) const { return !(*this == mat); }

	/// Inverse Jacobi Preconditioner Square
	/// Returns diag(A' * A)
	template <typename T> Vecvec<T> DiagATA() const;

	/// Jacobi Preconditioner Square
	/// Returns 1 / diag(A' * A)
	template <typename T> Vecvec<T> JacobiPreconditionerSquare() const;
};

// ################################################################################################
// Inline section
// ################################################################################################
// ------------------------------------------------------------------------------------------------
template <typename ValueType, typename SizeType>
bool CMatrix_CSR_Grid<ValueType, SizeType>::check_dimensions() const
{
	auto Ns = ns();
	auto Ms = ms();
	for (std::size_t i = 0; i < m_arr.n(); i++)
		for (std::size_t j = 0; j < m_arr.m(); j++)
			if ((m_arr(i,j).n() != Ns[i]) || (m_arr(i,j).m() != Ms[j]))
				return false;
	return true;
}

template <typename ValueType, typename SizeType>
CMatrix_CSR_Grid<ValueType, SizeType>::CMatrix_CSR_Grid(Array2D<Matrix> arr)
{
	m_arr = std::move(arr);
	assert(check_dimensions());
}

template <typename ValueType, typename SizeType>
CMatrix_CSR_Grid<ValueType, SizeType>::CMatrix_CSR_Grid(const Array2D<std::string>& filenames)
{
	load(filenames);
}

template <typename ValueType, typename SizeType>
void CMatrix_CSR_Grid<ValueType, SizeType>::load(const Array2D<std::string>& filenames)
{
	m_arr = Array2D<Matrix>(filenames.n(), filenames.m());
	for (std::size_t i = 0; i < filenames.n(); i++)
		for (std::size_t j = 0; j < filenames.m(); j++)
			m_arr(i,j) = Matrix(filenames(i,j));

	assert(check_dimensions());
}

template <typename ValueType, typename SizeType>
void CMatrix_CSR_Grid<ValueType, SizeType>::save(const Array2D<std::string>& filenames) const
{
	// Check dimenstions
	assert(m_arr.n() == filenames.n());
	assert(m_arr.m() == filenames.m());

	for (std::size_t i = 0; i < m_arr.n(); i++)
		for (std::size_t j = 0; j < m_arr.m(); j++)
			m_arr(i,j).save(filenames(i,j));
}

template <typename ValueType, typename SizeType>
std::size_t CMatrix_CSR_Grid<ValueType, SizeType>::nnz() const
{
	std::size_t sum = 0;
	for (std::size_t i = 0; i < m_arr.n(); i++)
		for (std::size_t j = 0; j < m_arr.m(); j++)
			sum += m_arr(i,j).nnz();
	return sum;
}

template <typename ValueType, typename SizeType>
std::size_t CMatrix_CSR_Grid<ValueType, SizeType>::size() const
{
	std::size_t sum = 0;
	for (std::size_t i = 0; i < m_arr.n(); i++)
		for (std::size_t j = 0; j < m_arr.m(); j++)
			sum += m_arr(i,j).size();
	return sum;
}

template <typename ValueType, typename SizeType>
std::vector<std::size_t> CMatrix_CSR_Grid<ValueType, SizeType>::ns() const
{
	std::vector<std::size_t> ret;
	ret.reserve(m_arr.n());
	for (std::size_t i = 0; i < m_arr.n(); i++)
		ret.push_back(m_arr(i,0).n());
	return ret;
}

template <typename ValueType, typename SizeType>
std::vector<std::size_t> CMatrix_CSR_Grid<ValueType, SizeType>::ms() const
{
	std::vector<std::size_t> ret;
	ret.reserve(m_arr.m());
	for (std::size_t j = 0; j < m_arr.m(); j++)
		ret.push_back(m_arr(0,j).m());
	return ret;
}

template <typename ValueType, typename SizeType>
template <typename T>
Vecvec<T> CMatrix_CSR_Grid<ValueType, SizeType>::Ax(const Vecvec<T>& x) const
{
	assert(ms() == x.ns());
	Vecvec<T> ret(ns());
	for (std::size_t i = 0; i < m_arr.n(); i++)
		for (std::size_t j = 0; j < m_arr.m(); j++)
			ret(i) += m_arr(i,j).Ax(x(j));
	return ret;
}

template <typename ValueType, typename SizeType>
template <typename T>
Vecvec<T> CMatrix_CSR_Grid<ValueType, SizeType>::ATx(const Vecvec<T>& x) const
{
	assert(ns() == x.ns());
	Vecvec<T> ret(ms());
	for (std::size_t i = 0; i < m_arr.n(); i++)
		for (std::size_t j = 0; j < m_arr.m(); j++)
			ret(j) += m_arr(i,j).ATx(x(i));
	return ret;
}

template <typename ValueType, typename SizeType>
template <typename T>
Vecvec<T> CMatrix_CSR_Grid<ValueType, SizeType>::ATAx(const Vecvec<T>& x) const
{
	return ATx(Ax(x));
}

template <typename ValueType, typename SizeType>
template <typename T>
Vecvec<T> CMatrix_CSR_Grid<ValueType, SizeType>::operator*(const Vecvec<T>& x) const
{
	return Ax(x);
}

template <typename ValueType, typename SizeType>
bool CMatrix_CSR_Grid<ValueType, SizeType>::operator==(const CMatrix_CSR_Grid<ValueType, SizeType>& mat) const
{
	for (std::size_t i = 0; i < m_arr.n(); i++)
		for (std::size_t j = 0; j < m_arr.m(); j++)
			if (m_arr(i,j) != mat.mat(i,j))
				return false;
	return true;
}

template <typename ValueType, typename SizeType>
template <typename T>
Vecvec<T> CMatrix_CSR_Grid<ValueType, SizeType>::DiagATA() const
{
	Vecvec<T> ret(ms(), 0);
	for (std::size_t j = 0; j < m_arr.m(); j++)
		for (std::size_t i = 0; i < m_arr.n(); i++)
			ret(j) += m_arr(i,j).template DiagATA<T>();
	return ret;
}

template <typename ValueType, typename SizeType>
template <typename T>
Vecvec<T> CMatrix_CSR_Grid<ValueType, SizeType>::JacobiPreconditionerSquare() const
{
	//Vecvec<T> ret(ms(), 0);
	//for (std::size_t i = 0; i < m_arr.n(); i++)
	//	for (std::size_t j = 0; j < m_arr.m(); j++)
	//		ret(j) += m_arr(i,j).JacobiPreconditionerSquare<T>();
	//return ret;
	return inv(DiagATA<T>());
}
// ------------------------------------------------------------------------------------------------
// ################################################################################################
