#include "gtest/gtest.h"
#include "array2D.h"
#include "matrixCSR_Grid.h"
#include "vector_extensions.h"
#include "vecvec.h"
#include <cstdint>
#include <vector>

TEST (matrixCSR_Grid, Interface)
{
	// 1 x | 2 x 1     1     12
	// 1 2 | x 1 x     2      9
	// ----+------     -     --
	// 2 x | x 1 x  *  3  ==  6
	// x 1 | 1 x 2     4     15
	// 1 3 | 0 x 3     5     22
	// x 1 | x x 1            7
	
	CMatrix_CSR<int> csr00(2);
	csr00.push_back(0, 1); csr00.endRow();
	csr00.push_back(0, 1); csr00.push_back(1, 2); csr00.endRow();
	
	CMatrix_CSR<int> csr01(3);
	csr01.push_back(0, 2); csr01.push_back(2, 1); csr01.endRow();
	csr01.push_back(1, 1); csr01.endRow();
	
	CMatrix_CSR<int> csr10(2);
	csr10.push_back(0, 2); csr10.endRow();
	csr10.push_back(1, 1); csr10.endRow();
	csr10.push_back(0, 1); csr10.push_back(1, 3); csr10.endRow();
	csr10.push_back(1, 1); csr10.endRow();
	
	CMatrix_CSR<int> csr11(3);
	csr11.push_back(1, 1); csr11.endRow();
	csr11.push_back(0, 1); csr11.push_back(2, 2); csr11.endRow();
	csr11.push_back(0, 0); csr11.push_back(2, 3); csr11.endRow();
	csr11.push_back(2, 1); csr11.endRow();
	
	Array2D<CMatrix_CSR<int>> arr(2,2);
	arr(0,0) = csr00;
	arr(0,1) = csr01;
	arr(1,0) = csr10;
	arr(1,1) = csr11;
	
	CMatrix_CSR_Grid<int> mat(std::move(arr));
	
	ASSERT_EQ (mat.n(), 2u);
	ASSERT_EQ (mat.m(), 2u);
	ASSERT_EQ (mat.nnz(), 16u);
	ASSERT_EQ (mat.size(), 17u);
	ASSERT_EQ (mat.ns()[0], 2u);
	ASSERT_EQ (mat.ns()[1], 4u);
	ASSERT_EQ (mat.ms()[0], 2u);
	ASSERT_EQ (mat.ms()[1], 3u);
	
	std::vector<int> v0;
	v0.push_back(1);
	v0.push_back(2);
	
	std::vector<int> v1;
	v1.push_back(3);
	v1.push_back(4);
	v1.push_back(5);
	
	Vecvec<int> vec(2);
	vec(0) = v0;
	vec(1) = v1;
	
	ASSERT_EQ (vec.ns()[0], 2u);
	ASSERT_EQ (vec.ns()[1], 3u);
	
	ASSERT_EQ (mat.ms(), vec.ns());
	
	auto x = mat * vec;
	ASSERT_EQ (mat.ns(), x.ns());
	ASSERT_EQ (x(0)[0], 12);
	ASSERT_EQ (x(0)[1], 9);
	ASSERT_EQ (x(1)[0], 6);
	ASSERT_EQ (x(1)[1], 15);
	ASSERT_EQ (x(1)[2], 22);
	ASSERT_EQ (x(1)[3], 7);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);	
	return RUN_ALL_TESTS();
}