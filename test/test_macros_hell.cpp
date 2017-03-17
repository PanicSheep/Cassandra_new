#include "macros_hell.h"
#include "gtest/gtest.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST (MacrosHellTest, BitScanLSB_1) {
	ASSERT_EQ (BitScanLSB(1), 0);
	ASSERT_EQ (BitScanLSB(2), 1);
	ASSERT_EQ (BitScanLSB(3), 0);
	ASSERT_EQ (BitScanLSB(0x8000000000000000ULL), 63);
}

TEST (MacrosHellTest, BitScanLSB_2) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanLSB(index, 1);
	ASSERT_EQ (ret, 0);
	ASSERT_EQ (index, 0);
	delete index;
}

TEST (MacrosHellTest, BitScanLSB_3) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanLSB(index, 2);
	ASSERT_EQ (ret, 0);
	ASSERT_EQ (index, 1);
	delete index;
}

TEST (MacrosHellTest, BitScanLSB_4) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanLSB(index, 3);
	ASSERT_EQ (ret, 0);
	ASSERT_EQ (index, 0);
	delete index;
}

TEST (MacrosHellTest, BitScanLSB_5) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanLSB(index, 0x8000000000000000ULL);
	ASSERT_EQ (ret, 0);
	ASSERT_EQ (index, 63);
	delete index;
}

TEST (MacrosHellTest, BitScanLSB_6) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanLSB(index, 0);
	ASSERT_EQ (ret, 1);
	delete index;
}
// --------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST (MacrosHellTest, BitScanMSB_1) {
	ASSERT_EQ (BitScanMSB(1), 0);
	ASSERT_EQ (BitScanMSB(2), 1);
	ASSERT_EQ (BitScanMSB(3), 1);
	ASSERT_EQ (BitScanMSB(0x8000000000000000ULL), 63);
}

TEST (MacrosHellTest,BitScanMSB_2) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanMSB(index, 1);
	ASSERT_EQ (ret, 0);
	ASSERT_EQ (index, 0);
	delete index;
}

TEST (MacrosHellTest, BitScanMSB_3) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanMSB(index, 2);
	ASSERT_EQ (ret, 0);
	ASSERT_EQ (index, 1);
	delete index;
}

TEST (MacrosHellTest, BitScanMSB_4) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanMSB(index, 3);
	ASSERT_EQ (ret, 0);
	ASSERT_EQ (index, 1);
	delete index;
}

TEST (MacrosHellTest, BitScanMSB_5) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanMSB(index, 0x8000000000000000ULL);
	ASSERT_EQ (ret, 0);
	ASSERT_EQ (index, 63);
	delete index;
}

TEST (MacrosHellTest, BitScanMSB_6) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanMSB(index, 0);
	ASSERT_EQ (ret, 1);
	delete index;
}
// --------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST (MacrosHellTest, CountLeadingZeros) {
	ASSERT_EQ (CountLeadingZeros(0), 64);
	ASSERT_EQ (CountLeadingZeros(1), 63);
	ASSERT_EQ (CountLeadingZeros(2), 62);
	ASSERT_EQ (CountLeadingZeros(3), 62);
	ASSERT_EQ (CountLeadingZeros(0x8000000000000000ULL), 0);
}

TEST (MacrosHellTest, CountTrailingZeros) {
	ASSERT_EQ (CountTrailingZeros(0), 64);
	ASSERT_EQ (CountTrailingZeros(1), 0);
	ASSERT_EQ (CountTrailingZeros(2), 1);
	ASSERT_EQ (CountTrailingZeros(3), 0);
	ASSERT_EQ (CountTrailingZeros(0x8000000000000000ULL), 63);
}
// --------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST (MacrosHellTest, GetLSB) {
	ASSERT_EQ (GetLSB(0), 0);
	ASSERT_EQ (GetLSB(1), 1);
	ASSERT_EQ (GetLSB(2), 2);
	ASSERT_EQ (GetLSB(3), 1);
	ASSERT_EQ (GetLSB(0x8000000000000000ULL), 0x8000000000000000ULL);
}

TEST (MacrosHellTest, GetMSB) {
	ASSERT_EQ (GetMSB(0), 0);
	ASSERT_EQ (GetMSB(1), 1);
	ASSERT_EQ (GetMSB(2), 2);
	ASSERT_EQ (GetMSB(3), 2);
	ASSERT_EQ (GetMSB(0x8000000000000000ULL), 0x8000000000000000ULL);
}

TEST (MacrosHellTest, RemoveLSB) {
	ASSERT_EQ (RemoveLSB(0), 0);
	ASSERT_EQ (RemoveLSB(1), 0);
	ASSERT_EQ (RemoveLSB(2), 0);
	ASSERT_EQ (RemoveLSB(3), 2);
	ASSERT_EQ (RemoveLSB(0x8000000001000000ULL), 0x8000000000000000ULL);
}

TEST (MacrosHellTest, RemoveMSB) {
	ASSERT_EQ (RemoveMSB(0), 0);
	ASSERT_EQ (RemoveMSB(1), 0);
	ASSERT_EQ (RemoveMSB(2), 0);
	ASSERT_EQ (RemoveMSB(3), 1);
	ASSERT_EQ (RemoveMSB(0x8000000001000000ULL), 0x0000000001000000ULL);
}
// --------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST (MacrosHellTest, PopCount) {
	ASSERT_EQ (PopCount(0), 0);
	ASSERT_EQ (PopCount(1), 1);
	ASSERT_EQ (PopCount(2), 1);
	ASSERT_EQ (PopCount(3), 2);
	ASSERT_EQ (PopCount(0xFFFFFFFFFFFFFFFFULL), 64);
}

TEST (MacrosHellTest, PopCount_max15) {
	ASSERT_EQ (PopCount_max15(0), 0);
	ASSERT_EQ (PopCount_max15(1), 1);
	ASSERT_EQ (PopCount_max15(2), 1);
	ASSERT_EQ (PopCount_max15(3), 2);
	ASSERT_EQ (PopCount_max15(0xFF000000000000FEULL), 15);
}
// --------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////

// TODO: Add test for BExtr
// TODO: Add test for BZHI
// TODO: Add test for PDep
// TODO: Add test for PExt
// TODO: Add test for BSwap

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
