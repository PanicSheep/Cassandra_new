#include "Array2D.h"
#include "ConfigFile.h"
#include "MatrixCSR.h"
#include "MatrixCSR_Grid.h"
#include "Path.h"
#include "Pattern.h"
#include "Position.h"
#include "PuzzleCollection.h"
#include "VecVec.h"
#include <cstdint>
#include <map>
#include <vector>

template <typename ValueType, typename SizeType>
CMatrix_CSR<ValueType, SizeType> to_Matrix(const PuzzleVector& puzzles, const uint64_t pattern)
{
	const std::size_t size = puzzles.size();

	auto Pattern = Pattern::Configurations::CreatePattern(pattern);
	CMatrix_CSR<ValueType, SizeType> A(Pattern->ReducedSize());

	#pragma omp parallel for ordered schedule(dynamic, 1)
	for (int64_t i = 0; i < static_cast<int64_t>(size); i++)
	{
		auto indices = Pattern->Configurations(puzzles[i]->GetPosition());

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
std::vector<VectorValueType> to_Vector(const PuzzleVector& puzzles)
{
	const std::size_t size = puzzles.size();
	std::vector<VectorValueType> Score;
	Score.resize(size);

	#pragma omp parallel for schedule(dynamic, 256)
	for (int64_t i = 0; i < static_cast<int64_t>(size); i++)
	{
		Score[i] = dynamic_cast<CPuzzleScore*>(puzzles[i].get())->score;
	}

	return Score;
}

//template <typename ValueType, typename SizeType, typename VectorValueType>
//std::pair<CMatrix_CSR_Grid<ValueType, SizeType>, Vecvec<VectorValueType>> PositionToMatrix(const Vecvec<CPositionScore>& pos, const std::vector<std::string>& name)
//{
//	std::vector<uint64_t> pattern;
//	for (const auto& it : name)
//		pattern.push_back(m_pattern::LookupPattern(it));
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
