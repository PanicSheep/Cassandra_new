#include "Array2D.h"
#include "BoardCollection.h"
#include "ConfigFile.h"
#include "MatrixCSR.h"
#include "MatrixCSR_Grid.h"
#include "Position.h"
#include "Pattern.h"
#include "Path.h"
#include "VecVec.h"
#include <vector>
#include <map>
#include <cstdint>

using namespace IO;

template <typename ValueType, typename SizeType>
CMatrix_CSR<ValueType, SizeType> to_Matrix(const CBoardCollection& boards, const uint64_t pattern)
{
	const std::size_t size = boards.size();

	auto Pattern = CreatePattern(pattern);
	CMatrix_CSR<ValueType, SizeType> A(Pattern->ReducedSize());

	#pragma omp parallel for ordered schedule(static, 1)
	for (int64_t i = 0; i < static_cast<int64_t>(size); i++)
	{
		auto indices = Pattern->GetConfigurations(boards.Get(i)->GetPosition());

		std::map<SizeType, ValueType> map;
		for (SizeType it : indices)
			map[it]++;

		#pragma omp ordered
		{
			for (const auto& it : map)
				A.push_back(it.first, it.second);
			A.endRow();
		}
	}

	return A;
}

template <typename VectorValueType>
std::vector<VectorValueType> to_Vector(const CBoardCollection& boards)
{
	const std::size_t size = boards.size();
	std::vector<VectorValueType> Score;
	Score.reserve(size);

	#pragma omp parallel for schedule(static, 256)
	for (int64_t i = 0; i < static_cast<int64_t>(size); i++)
	{
		Score[i] = dynamic_cast<CBoardScore*>(boards.Get(i).get())->score;
	}

	return Score;
}

//template <typename ValueType, typename SizeType, typename VectorValueType>
//std::pair<CMatrix_CSR_Grid<ValueType, SizeType>, Vecvec<VectorValueType>> PositionToMatrix(const Vecvec<CPositionScore>& pos, const std::vector<std::string>& name)
//{
//	std::vector<uint64_t> pattern;
//	for (const auto& it : name)
//		pattern.push_back(Pattern::LookupPattern(it));
//	return PositionToMatrix<ValueType, SizeType, VectorValueType>(pos, pattern);
//}
//
//template <typename ValueType, typename SizeType, typename VectorValueType>
//std::pair<CMatrix_CSR_Grid<ValueType, SizeType>, Vecvec<VectorValueType>> PositionToMatrix(const Vecvec<CPositionScore>& pos, const std::vector<uint64_t> pattern)
//{
//	Array2D<CMatrix_CSR<ValueType, SizeType>> arr(pos.n(), pattern.size());
//	Vecvec<VectorValueType> vecvec(pos.n());
//	std::pair<CMatrix_CSR<ValueType, SizeType>, std::vector<VectorValueType>> matvec;
//
//	for (std::size_t i = 0; i < pos.n(); i++)
//	{
//		for (std::size_t j = 0; j < pattern.size(); j++)
//		{
//			matvec = PositionToMatrix<ValueType, SizeType, VectorValueType>(pos(i), pattern[j]);
//			arr(i,j) = std::move(matvec.first);
//		}
//		vecvec(i) = matvec.second;
//	}
//
//	return std::pair<CMatrix_CSR_Grid<ValueType, SizeType>, Vecvec<VectorValueType>>(arr, vecvec);
//}
