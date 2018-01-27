#include "gtest/gtest.h"
#include "matrixCSR.h"
#include "iterativealgorithms.h"
#include "vector_extensions.h"
#include "statistics.h"
#include "position.h"
#include "generatepositions.h"
#include "pattern.h"
#include "positiontomatrix.h"
#include "game.h"
#include <cstdint>
#include <functional>
#include <random>
#include <vector>

std::vector<CPositionScore> MakeSolvedRandomPosition(const std::size_t size, const std::size_t empties)
{
	std::vector<CPositionScore> pos;
	
	auto rndposset = GenerateRandomPositions(size, empties, false, 1234);
	std::vector<CPosition> rndpos(rndposset.begin(), rndposset.end());
	
	for (std::size_t i = 0; i < rndpos.size(); i++)
	{
		auto score = PVS::Eval(rndpos[i].P, rndpos[i].O);
		pos.push_back(CPositionScore(rndpos[i].P, rndpos[i].O, score));
	}
	
	return pos; 
}

class SmallMatrix : public ::testing::Test
{
public:
	static CMatrix_CSR<uint8_t, uint32_t> A;
	static std::vector<float> x_star, x0, b;
protected:	
	static void SetUpTestCase()
	{
		// 1,5
		// 7,2
		// 1,6
		A = CMatrix_CSR<uint8_t, uint32_t>(2);
		A.push_back(0,1); A.push_back(1,3); A.endRow();
		A.push_back(0,7); A.push_back(1,1); A.endRow();
		A.push_back(0,1); A.push_back(1,6); A.endRow();

		x_star = {1, 3};
		b = A * x_star;
		x0 = {0, 0};
		
		ASSERT_EQ (A * x_star, b);
	}
	
	static void TearDownTestCase() {}
	
	void MyTest(CIterativeAlgorithm<CMatrix_CSR<uint8_t, uint32_t>,std::vector<float>,float>* alg)
	{
		alg->Iterate(100);
		auto x = alg->GetX();

		ASSERT_NEAR (x[0], x_star[0], 1e-3);
		ASSERT_NEAR (x[1], x_star[1], 1e-3);
		ASSERT_EQ (alg->GetTerminationCause(), eTerminationCause::Converged);
		delete alg;
	}
};

CMatrix_CSR<uint8_t, uint32_t> SmallMatrix::A;
std::vector<float> SmallMatrix::x_star, SmallMatrix::x0, SmallMatrix::b;


TEST_F (SmallMatrix, CG)
{
	auto a0 = A.ATAx(std::vector<float>({1,0}));
	auto a1 = A.ATAx(std::vector<float>({0,1}));
	auto ATA = CMatrix_CSR<uint8_t, uint32_t>(2);
	ATA.push_back(0,a0[0]); ATA.push_back(1,a0[1]); ATA.endRow();
	ATA.push_back(0,a1[0]); ATA.push_back(1,a1[1]); ATA.endRow();
	
	std::vector<float> ATb = A.ATx(b);
	
	MyTest(new CIterativeCG<CMatrix_CSR<uint8_t, uint32_t>,std::vector<float>,float>(ATA, ATb, x0, 1e-3));
}
	
TEST_F (SmallMatrix, PCG)
{
	auto a0 = A.ATAx(std::vector<float>({1,0}));
	auto a1 = A.ATAx(std::vector<float>({0,1}));
	auto ATA = CMatrix_CSR<uint8_t, uint32_t>(2);
	ATA.push_back(0,a0[0]); ATA.push_back(1,a0[1]); ATA.endRow();
	ATA.push_back(0,a1[0]); ATA.push_back(1,a1[1]); ATA.endRow();
	
	std::vector<float> ATb = A.ATx(b);
	
	CDiagonalPreconditioner<CMatrix_CSR<uint8_t, uint32_t>, std::vector<float>> P(sqrt(ATA.JacobiPreconditionerSquare<float>()));
	MyTest(new CIterativePCG<CMatrix_CSR<uint8_t, uint32_t>,std::vector<float>,float>(ATA, &P, ATb, x0, 1e-3));
}

TEST_F (SmallMatrix, CGLS)
{
	MyTest(new CIterativeCGLS<CMatrix_CSR<uint8_t, uint32_t>,std::vector<float>,float>(A, b, x0, 1e-3));
}

TEST_F (SmallMatrix, PCGLS)
{
	CDiagonalPreconditioner<CMatrix_CSR<uint8_t, uint32_t>, std::vector<float>> P(sqrt(A.JacobiPreconditionerSquare<float>()));
	MyTest(new CIterativePCGLS<CMatrix_CSR<uint8_t, uint32_t>,std::vector<float>,float>(A, &P, b, x0, 1e-3));
}

TEST_F (SmallMatrix, LSQR)
{
	MyTest(new CIterativeLSQR<CMatrix_CSR<uint8_t, uint32_t>,std::vector<float>,float>(A, b, x0, 1e-3));
}

TEST_F (SmallMatrix, PLSQR)
{
	CDiagonalPreconditioner<CMatrix_CSR<uint8_t, uint32_t>, std::vector<float>> P(sqrt(A.JacobiPreconditionerSquare<float>()));
	MyTest(new CIterativePLSQR<CMatrix_CSR<uint8_t, uint32_t>,std::vector<float>,float>(A, &P, b, x0, 1e-3));
}

class RandomMatrix : public ::testing::Test
{
public:
	static CMatrix_CSR<uint8_t, uint32_t> A;
	static std::vector<float> x_star, x0, b;
protected:
	static CMatrix_CSR<uint8_t, uint32_t> MakeMatrix(const std::size_t n, const std::size_t m, const float density);
	static std::vector<float> MakeVector(const std::size_t size);
	static void SetUpTestCase()
	{
		A = MakeMatrix(100000, 1000, 0.01);
		x_star = MakeVector(1000);
		b = A * x_star;
		x0 = std::vector<float>(1000, 0);
		
		ASSERT_EQ (A * x_star, b);
	}
	
	static void TearDownTestCase() {}
	
	void MyTest(CIterativeAlgorithm<CMatrix_CSR<uint8_t, uint32_t>,std::vector<float>,float>* alg)
	{
		alg->Iterate(100);
		auto x = alg->GetX();

		ASSERT_EQ (alg->GetTerminationCause(), eTerminationCause::Converged);
		ASSERT_TRUE (norm(x - x_star) / norm(x_star) < 1e-3);
		delete alg;
	}
};

CMatrix_CSR<uint8_t, uint32_t> RandomMatrix::A;
std::vector<float> RandomMatrix::x_star, RandomMatrix::x0, RandomMatrix::b;

CMatrix_CSR<uint8_t, uint32_t> RandomMatrix::MakeMatrix(const std::size_t n, const std::size_t m, const float density)
{
	auto rndData = std::bind(std::uniform_int_distribution<uint32_t>(0,4), std::mt19937_64(143));
	auto rndCol = std::bind(std::uniform_int_distribution<uint32_t>(0,m-1), std::mt19937_64(123));
	CMatrix_CSR<uint8_t, uint32_t> A(m);
	for (std::size_t i = 0; i < n; i++)
	{
		std::vector<uint32_t> col;
		std::vector<uint8_t> data;
		for (std::size_t j = 0; j < static_cast<std::size_t>(m * density); j++)
		{
			col.push_back(rndCol());
			data.push_back(rndData());
		}
		std::sort(col.begin(), col.end());
		
		A.push_back(col, data);
	}
	return A;
}

std::vector<float> RandomMatrix::MakeVector(const std::size_t size)
{
	auto rnd = std::bind(std::uniform_int_distribution<int32_t>(-20,+20), std::mt19937_64(122));
	std::vector<float> x;
	for (std::size_t i = 0; i < size; i++)
		x.push_back(rnd());
	return x;
}

TEST_F (RandomMatrix, CGLS)
{
	MyTest(new CIterativeCGLS<CMatrix_CSR<uint8_t, uint32_t>,std::vector<float>,float>(A, b, x0, 1e-3));
}

TEST_F (RandomMatrix, PCGLS)
{
	CDiagonalPreconditioner<CMatrix_CSR<uint8_t, uint32_t>, std::vector<float>> P(sqrt(A.JacobiPreconditionerSquare<float>()));
	MyTest(new CIterativePCGLS<CMatrix_CSR<uint8_t, uint32_t>,std::vector<float>,float>(A, &P, b, x0, 1e-3));
}

TEST_F (RandomMatrix, LSQR)
{
	MyTest(new CIterativeLSQR<CMatrix_CSR<uint8_t, uint32_t>,std::vector<float>,float>(A, b, x0, 1e-3));
}

TEST_F (RandomMatrix, PLSQR)
{
	CDiagonalPreconditioner<CMatrix_CSR<uint8_t, uint32_t>, std::vector<float>> P(sqrt(A.JacobiPreconditionerSquare<float>()));
	MyTest(new CIterativePLSQR<CMatrix_CSR<uint8_t, uint32_t>,std::vector<float>,float>(A, &P, b, x0, 1e-3));
}


class SolvedRandomPosition : public ::testing::Test
{
public:
	static std::vector<CPositionScore> pos;
protected:
	static void SetUpTestCase()
	{
		pos = MakeSolvedRandomPosition(100000, 5);
	}
	
	static void TearDownTestCase() {}
		
	void MyTest_CGLS(uint64_t pattern)
	{
		auto ret = PositionToMatrix<uint8_t, uint32_t, float>(pos, pattern);
		auto A = ret.first;
		auto b = ret.second;
		std::vector<float> x0(A.m(), 0);
		auto alg = new CIterativeCGLS<CMatrix_CSR<uint8_t, uint32_t>,std::vector<float>,float>(A, b, x0, 1);
		for (int i = 0; i < 100; i++) {
			alg->Iterate(100);
			if (alg->GetTerminationCause() == eTerminationCause::Converged)
				break;
		}
		auto x = alg->GetX();

		ASSERT_EQ (alg->GetTerminationCause(), eTerminationCause::Converged);
		ASSERT_LT (StandardDeviation(A * x - b), StandardDeviation(b));
		delete alg;
	}
		
	void MyTest_PCGLS(uint64_t pattern)
	{
		auto ret = PositionToMatrix<uint8_t, uint32_t, float>(pos, pattern);
		auto A = ret.first;
		auto b = ret.second;
		std::vector<float> x0(A.m(), 0);
		CDiagonalPreconditioner<CMatrix_CSR<uint8_t, uint32_t>, std::vector<float>> P(sqrt(A.JacobiPreconditionerSquare<float>()));
		auto alg = new CIterativePCGLS<CMatrix_CSR<uint8_t, uint32_t>,std::vector<float>,float>(A, &P, b, x0, 1);
		for (int i = 0; i < 100; i++) {
			alg->Iterate(100);
			if (alg->GetTerminationCause() == eTerminationCause::Converged)
				break;
		}
		auto x = alg->GetX();

		ASSERT_EQ (alg->GetTerminationCause(), eTerminationCause::Converged);
		ASSERT_LT(StandardDeviation(A * x - b), StandardDeviation(b));
		delete alg;
	}
	
	void MyTest_LSQR(uint64_t pattern)
	{
		auto ret = PositionToMatrix<uint8_t, uint32_t, float>(pos, pattern);
		auto A = ret.first;
		auto b = ret.second;
		std::vector<float> x0(A.m(), 0);
		auto alg = new CIterativeLSQR<CMatrix_CSR<uint8_t, uint32_t>,std::vector<float>,float>(A, b, x0, 1);
		for (int i = 0; i < 100; i++) {
			alg->Iterate(100);
			if (alg->GetTerminationCause() == eTerminationCause::Converged)
				break;
		}
		auto x = alg->GetX();

		ASSERT_EQ (alg->GetTerminationCause(), eTerminationCause::Converged);
		ASSERT_LT(StandardDeviation(A * x - b), StandardDeviation(b));
		delete alg;
	}
		
	void MyTest_PLSQR(uint64_t pattern)
	{
		auto ret = PositionToMatrix<uint8_t, uint32_t, float>(pos, pattern);
		auto A = ret.first;
		auto b = ret.second;
		std::vector<float> x0(A.m(), 0);
		CDiagonalPreconditioner<CMatrix_CSR<uint8_t, uint32_t>, std::vector<float>> P(sqrt(A.JacobiPreconditionerSquare<float>()));
		auto alg = new CIterativePLSQR<CMatrix_CSR<uint8_t, uint32_t>,std::vector<float>,float>(A, &P, b, x0, 1);
		for (int i = 0; i < 100; i++) {
			alg->Iterate(100);
			if (alg->GetTerminationCause() == eTerminationCause::Converged)
				break;
		}
		auto x = alg->GetX();

		ASSERT_EQ (alg->GetTerminationCause(), eTerminationCause::Converged);
		ASSERT_LT(StandardDeviation(A * x - b), StandardDeviation(b));
		delete alg;
	}
};

std::vector<CPositionScore> SolvedRandomPosition::pos;

TEST_F (SolvedRandomPosition, CGLS_1) { MyTest_CGLS(0x00000000000000FFULL); }
TEST_F (SolvedRandomPosition, CGLS_2) { MyTest_CGLS(0x000000000000FF00ULL); }
TEST_F (SolvedRandomPosition, CGLS_3) { MyTest_CGLS(0x0000000000FF0000ULL); }
TEST_F (SolvedRandomPosition, CGLS_4) { MyTest_CGLS(0x00000000FF000000ULL); }
TEST_F (SolvedRandomPosition, CGLS_5) { MyTest_CGLS(0x0000000000010204ULL); }
TEST_F (SolvedRandomPosition, CGLS_6) { MyTest_CGLS(0x0000000001020408ULL); }
TEST_F (SolvedRandomPosition, CGLS_7) { MyTest_CGLS(0x0000000102040810ULL); }
TEST_F (SolvedRandomPosition, CGLS_8) { MyTest_CGLS(0x0000010204081020ULL); }
TEST_F (SolvedRandomPosition, CGLS_9) { MyTest_CGLS(0x0001020408102040ULL); }
TEST_F (SolvedRandomPosition, CGLS_10) { MyTest_CGLS(0x0102040810204080ULL); }
TEST_F (SolvedRandomPosition, CGLS_13) { MyTest_CGLS(0x0000000000070707ULL); }
TEST_F (SolvedRandomPosition, CGLS_23) { MyTest_CGLS(0x8040201008040303ULL); }

TEST_F (SolvedRandomPosition, PCGLS_1) { MyTest_PCGLS(0x00000000000000FFULL); }
TEST_F (SolvedRandomPosition, PCGLS_2) { MyTest_PCGLS(0x000000000000FF00ULL); }
TEST_F (SolvedRandomPosition, PCGLS_3) { MyTest_PCGLS(0x0000000000FF0000ULL); }
TEST_F (SolvedRandomPosition, PCGLS_4) { MyTest_PCGLS(0x00000000FF000000ULL); }
TEST_F (SolvedRandomPosition, PCGLS_5) { MyTest_PCGLS(0x0000000000010204ULL); }
TEST_F (SolvedRandomPosition, PCGLS_6) { MyTest_PCGLS(0x0000000001020408ULL); }
TEST_F (SolvedRandomPosition, PCGLS_7) { MyTest_PCGLS(0x0000000102040810ULL); }
TEST_F (SolvedRandomPosition, PCGLS_8) { MyTest_PCGLS(0x0000010204081020ULL); }
TEST_F (SolvedRandomPosition, PCGLS_9) { MyTest_PCGLS(0x0001020408102040ULL); }
TEST_F (SolvedRandomPosition, PCGLS_10) { MyTest_PCGLS(0x0102040810204080ULL); }
TEST_F (SolvedRandomPosition, PCGLS_13) { MyTest_PCGLS(0x0000000000070707ULL); }
TEST_F (SolvedRandomPosition, PCGLS_23) { MyTest_PCGLS(0x8040201008040303ULL); }

TEST_F (SolvedRandomPosition, LSQR_1) { MyTest_LSQR(0x00000000000000FFULL); }
TEST_F (SolvedRandomPosition, LSQR_2) { MyTest_LSQR(0x000000000000FF00ULL); }
TEST_F (SolvedRandomPosition, LSQR_3) { MyTest_LSQR(0x0000000000FF0000ULL); }
TEST_F (SolvedRandomPosition, LSQR_4) { MyTest_LSQR(0x00000000FF000000ULL); }
TEST_F (SolvedRandomPosition, LSQR_5) { MyTest_LSQR(0x0000000000010204ULL); }
TEST_F (SolvedRandomPosition, LSQR_6) { MyTest_LSQR(0x0000000001020408ULL); }
TEST_F (SolvedRandomPosition, LSQR_7) { MyTest_LSQR(0x0000000102040810ULL); }
TEST_F (SolvedRandomPosition, LSQR_8) { MyTest_LSQR(0x0000010204081020ULL); }
TEST_F (SolvedRandomPosition, LSQR_9) { MyTest_LSQR(0x0001020408102040ULL); }
TEST_F (SolvedRandomPosition, LSQR_10) { MyTest_LSQR(0x0102040810204080ULL); }
TEST_F (SolvedRandomPosition, LSQR_13) { MyTest_LSQR(0x0000000000070707ULL); }
TEST_F (SolvedRandomPosition, LSQR_23) { MyTest_LSQR(0x8040201008040303ULL); }

TEST_F (SolvedRandomPosition, PLSQR_1) { MyTest_PLSQR(0x00000000000000FFULL); }
TEST_F (SolvedRandomPosition, PLSQR_2) { MyTest_PLSQR(0x000000000000FF00ULL); }
TEST_F (SolvedRandomPosition, PLSQR_3) { MyTest_PLSQR(0x0000000000FF0000ULL); }
TEST_F (SolvedRandomPosition, PLSQR_4) { MyTest_PLSQR(0x00000000FF000000ULL); }
TEST_F (SolvedRandomPosition, PLSQR_5) { MyTest_PLSQR(0x0000000000010204ULL); }
TEST_F (SolvedRandomPosition, PLSQR_6) { MyTest_PLSQR(0x0000000001020408ULL); }
TEST_F (SolvedRandomPosition, PLSQR_7) { MyTest_PLSQR(0x0000000102040810ULL); }
TEST_F (SolvedRandomPosition, PLSQR_8) { MyTest_PLSQR(0x0000010204081020ULL); }
TEST_F (SolvedRandomPosition, PLSQR_9) { MyTest_PLSQR(0x0001020408102040ULL); }
TEST_F (SolvedRandomPosition, PLSQR_10) { MyTest_PLSQR(0x0102040810204080ULL); }
TEST_F (SolvedRandomPosition, PLSQR_13) { MyTest_PLSQR(0x0000000000070707ULL); }
TEST_F (SolvedRandomPosition, PLSQR_23) { MyTest_PLSQR(0x8040201008040303ULL); }


class SolvedRandomPosition_Grid : public ::testing::Test
{
public:
	static Vecvec<CPositionScore> pos;
protected:
	static void SetUpTestCase()
	{
		pos = Vecvec<CPositionScore>(5);
		pos(0) = MakeSolvedRandomPosition(10000, 4);
		pos(1) = MakeSolvedRandomPosition(10000, 5);
		pos(2) = MakeSolvedRandomPosition(10000, 6);
		pos(3) = MakeSolvedRandomPosition(10000, 7);
		pos(4) = MakeSolvedRandomPosition(10000, 8);
	}
	
	static void TearDownTestCase() {}
		
	void MyTest_CGLS(std::vector<std::string> pattern)
	{
		auto ret = PositionToMatrix<uint8_t, uint32_t, float>(pos, pattern);
		auto A = ret.first;
		auto b = ret.second;
		Vecvec<float> x0(A.ms(), 0);
		auto alg = new CIterativeCGLS<CMatrix_CSR_Grid<uint8_t, uint32_t>, Vecvec<float>,float>(A, b, x0, 1);
		for (int i = 0; i < 100; i++) {
			alg->Iterate(100);
			if (alg->GetTerminationCause() == eTerminationCause::Converged)
				break;
		}
		auto x = alg->GetX();

		ASSERT_EQ (alg->GetTerminationCause(), eTerminationCause::Converged);
		ASSERT_LT (StandardDeviation(A * x - b), StandardDeviation(b));
		delete alg;
	}
		
	void MyTest_PCGLS(std::vector<std::string> pattern)
	{
		auto ret = PositionToMatrix<uint8_t, uint32_t, float>(pos, pattern);
		auto A = ret.first;
		auto b = ret.second;
		Vecvec<float> x0(A.ms(), 0);
		CDiagonalPreconditioner<CMatrix_CSR_Grid<uint8_t, uint32_t>, Vecvec<float>> P(sqrt(A.JacobiPreconditionerSquare<float>()));
		auto alg = new CIterativePCGLS<CMatrix_CSR_Grid<uint8_t, uint32_t>, Vecvec<float>, float>(A, &P, b, x0, 1);
		for (int i = 0; i < 100; i++) {
			alg->Iterate(100);
			if (alg->GetTerminationCause() == eTerminationCause::Converged)
				break;
		}
		auto x = alg->GetX();

		ASSERT_EQ (alg->GetTerminationCause(), eTerminationCause::Converged);
		ASSERT_LT(StandardDeviation(A * x - b), StandardDeviation(b));
		delete alg;
	}
	
	void MyTest_LSQR(std::vector<std::string> pattern)
	{
		auto ret = PositionToMatrix<uint8_t, uint32_t, float>(pos, pattern);
		auto A = ret.first;
		auto b = ret.second;
		Vecvec<float> x0(A.ms(), 0);
		auto alg = new CIterativeLSQR<CMatrix_CSR_Grid<uint8_t, uint32_t>, Vecvec<float>, float>(A, b, x0, 1);
		for (int i = 0; i < 100; i++) {
			alg->Iterate(100);
			if (alg->GetTerminationCause() == eTerminationCause::Converged)
				break;
		}
		auto x = alg->GetX();

		ASSERT_EQ (alg->GetTerminationCause(), eTerminationCause::Converged);
		ASSERT_LT(StandardDeviation(A * x - b), StandardDeviation(b));
		delete alg;
	}
	
	void MyTest_PLSQR(std::vector<std::string> pattern)
	{
		auto ret = PositionToMatrix<uint8_t, uint32_t, float>(pos, pattern);
		auto A = ret.first;
		auto b = ret.second;
		Vecvec<float> x0(A.ms(), 0);
		CDiagonalPreconditioner<CMatrix_CSR_Grid<uint8_t, uint32_t>, Vecvec<float>> P(sqrt(A.JacobiPreconditionerSquare<float>()));
		auto alg = new CIterativePLSQR<CMatrix_CSR_Grid<uint8_t, uint32_t>, Vecvec<float>, float>(A, &P, b, x0, 1);
		for (int i = 0; i < 100; i++) {
			alg->Iterate(100);
			if (alg->GetTerminationCause() == eTerminationCause::Converged)
				break;
		}
		auto x = alg->GetX();
		
		ASSERT_EQ (alg->GetTerminationCause(), eTerminationCause::Converged);
		ASSERT_LT(StandardDeviation(A * x - b), StandardDeviation(b));
		delete alg;
	}
};

Vecvec<CPositionScore> SolvedRandomPosition_Grid::pos(5);

TEST_F (SolvedRandomPosition_Grid,  CGLS_1) { MyTest_CGLS (std::vector<std::string>{ "L0", "L1", "L2", "L3" }); }
TEST_F (SolvedRandomPosition_Grid, PCGLS_1) { MyTest_PCGLS(std::vector<std::string>{ "L0", "L1", "L2", "L3" }); }
TEST_F (SolvedRandomPosition_Grid,  LSQR_1) { MyTest_LSQR (std::vector<std::string>{ "L0", "L1", "L2", "L3" }); }
TEST_F (SolvedRandomPosition_Grid, PLSQR_1) { MyTest_PLSQR(std::vector<std::string>{ "L0", "L1", "L2", "L3" }); }

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	
	Configfile::Initialize(argv[0]);
	CountLastFlip::Initialize();
	Stability::Initialize();
	Pattern::Initialize(false, true);
	gTT = CHashTable(32 * 1024 * 1024);
	gTTPV = CHashTable(1024 * 1024);
	
	return RUN_ALL_TESTS();
}