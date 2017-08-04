#include "array2D.h"
#include "configfile.h"
#include "matrixCSR.h"
#include "matrixCSR_Grid.h"
#include "position.h"
#include "pattern.h"
#include "vecvec.h"
#include <vector>
#include <map>
#include <cstdint>

template <typename ValueType, typename SizeType, typename VectorValueType>
std::pair<CMatrix_CSR<ValueType, SizeType>, std::vector<VectorValueType>> PositionToMatrix(const std::vector<CPositionScore>& pos, const std::string& name);

template <typename ValueType, typename SizeType, typename VectorValueType>
std::pair<CMatrix_CSR<ValueType, SizeType>, std::vector<VectorValueType>> PositionToMatrix(const std::vector<CPositionScore>& pos, const uint64_t pattern);

template <typename ValueType, typename SizeType, typename VectorValueType>
std::pair<CMatrix_CSR_Grid<ValueType, SizeType>, Vecvec<VectorValueType>> PositionToMatrix(const Array2D<CPositionScore>& pos, const std::vector<std::string>& name);

template <typename ValueType, typename SizeType, typename VectorValueType>
std::pair<CMatrix_CSR_Grid<ValueType, SizeType>, Vecvec<VectorValueType>> PositionToMatrix(const Array2D<CPositionScore>& pos, const std::vector<uint64_t> pattern);


// ################################################################################################
// Implementation
// ################################################################################################
// ------------------------------------------------------------------------------------------------
template <typename ValueType, typename SizeType, typename VectorValueType>
std::pair<CMatrix_CSR<ValueType, SizeType>, std::vector<VectorValueType>> PositionToMatrix(const std::vector<CPositionScore>& pos, const std::string& name)
{
	const auto pattern = Pattern::LookupPattern(name);
	return PositionToMatrix<ValueType, SizeType, VectorValueType>(pos, pattern);
}

template <typename ValueType, typename SizeType, typename VectorValueType>
std::pair<CMatrix_CSR<ValueType, SizeType>, std::vector<VectorValueType>> PositionToMatrix(const std::vector<CPositionScore>& pos, const uint64_t pattern)
{
	const std::size_t size = pos.size();
	
	Pattern::CPattern * Pattern = Pattern::NewPattern(std::to_string(pattern), pattern);
	CMatrix_CSR<ValueType, SizeType> A(Pattern->ReducedSize());
	std::vector<VectorValueType> Score;
	Score.reserve(size);
	
	//#pragma omp parallel for schedule(static, 256)
	for (int64_t i = 0; i < static_cast<int64_t>(size); i++)
	{
		auto indices = Pattern->GetConfigurationVec(pos[i].P, pos[i].O);

		std::map<SizeType, ValueType> map;
		for (const auto& it : indices)
			map[it]++;

		#pragma omp critical
		{
			for (const auto& it : map)
				A.push_back(it.first, it.second);
			A.endRow();
			Score.push_back(pos[i].score);
		}
	}
	
	return std::pair<CMatrix_CSR<ValueType, SizeType>, std::vector<VectorValueType>>(A, Score);
}

template <typename ValueType, typename SizeType, typename VectorValueType>
std::pair<CMatrix_CSR_Grid<ValueType, SizeType>, Vecvec<VectorValueType>> PositionToMatrix(const Vecvec<CPositionScore>& pos, const std::vector<std::string>& name)
{
	std::vector<uint64_t> pattern;
	for (const auto& it : name)
		pattern.push_back(Pattern::LookupPattern(it));
	return PositionToMatrix<ValueType, SizeType, VectorValueType>(pos, pattern);
}

template <typename ValueType, typename SizeType, typename VectorValueType>
std::pair<CMatrix_CSR_Grid<ValueType, SizeType>, Vecvec<VectorValueType>> PositionToMatrix(const Vecvec<CPositionScore>& pos, const std::vector<uint64_t> pattern)
{
	Array2D<CMatrix_CSR<ValueType, SizeType>> arr(pos.n(), pattern.size());
	Vecvec<VectorValueType> vecvec(pos.n());
	std::pair<CMatrix_CSR<ValueType, SizeType>, std::vector<VectorValueType>> matvec;
	
	for (std::size_t i = 0; i < pos.n(); i++)
	{
		for (std::size_t j = 0; j < pattern.size(); j++)
		{
			matvec = PositionToMatrix<ValueType, SizeType, VectorValueType>(pos(i), pattern[j]);
			arr(i,j) = std::move(matvec.first);
		}
		vecvec(i) = matvec.second;
	}
	
	return std::pair<CMatrix_CSR_Grid<ValueType, SizeType>, Vecvec<VectorValueType>>(arr, vecvec);
}
// ------------------------------------------------------------------------------------------------
// ################################################################################################
