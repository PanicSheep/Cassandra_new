#include "macros_hell.h"
#include "gtest/gtest.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST (MacrosHellTest, BitScanLSB_1) {
	ASSERT_DEATH (BitScanLSB(0), "(.*?)");
	ASSERT_EQ (BitScanLSB(1), 0);
	ASSERT_EQ (BitScanLSB(2), 1);
	ASSERT_EQ (BitScanLSB(3), 0);
	ASSERT_EQ (BitScanLSB(0x8000000000000000ULL), 63);
}

TEST (MacrosHellTest, BitScanLSB_2) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanLSB(index, 1);
	ASSERT_NE (ret, 0);
	ASSERT_EQ (*index, 0);
	delete index;
}

TEST (MacrosHellTest, BitScanLSB_3) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanLSB(index, 2);
	ASSERT_NE (ret, 0);
	ASSERT_EQ (*index, 1);
	delete index;
}

TEST (MacrosHellTest, BitScanLSB_4) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanLSB(index, 3);
	ASSERT_NE (ret, 0);
	ASSERT_EQ (*index, 0);
	delete index;
}

TEST (MacrosHellTest, BitScanLSB_5) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanLSB(index, 0x8000000000000000ULL);
	ASSERT_NE (ret, 0);
	ASSERT_EQ (*index, 63);
	delete index;
}

TEST (MacrosHellTest, BitScanLSB_6) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanLSB(index, 0);
	ASSERT_EQ (ret, 0);
	delete index;
}
// --------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST (MacrosHellTest, BitScanMSB_1) {
	ASSERT_DEATH (BitScanMSB(0), "(.*?)");
	ASSERT_EQ (BitScanMSB(1), 0);
	ASSERT_EQ (BitScanMSB(2), 1);
	ASSERT_EQ (BitScanMSB(3), 1);
	ASSERT_EQ (BitScanMSB(0x8000000000000000ULL), 63);
}

TEST (MacrosHellTest,BitScanMSB_2) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanMSB(index, 1);
	ASSERT_NE (ret, 0);
	ASSERT_EQ (*index, 0);
	delete index;
}

TEST (MacrosHellTest, BitScanMSB_3) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanMSB(index, 2);
	ASSERT_NE (ret, 0);
	ASSERT_EQ (*index, 1);
	delete index;
}

TEST (MacrosHellTest, BitScanMSB_4) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanMSB(index, 3);
	ASSERT_NE (ret, 0);
	ASSERT_EQ (*index, 1);
	delete index;
}

TEST (MacrosHellTest, BitScanMSB_5) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanMSB(index, 0x8000000000000000ULL);
	ASSERT_NE (ret, 0);
	ASSERT_EQ (*index, 63);
	delete index;
}

TEST (MacrosHellTest, BitScanMSB_6) {
	unsigned long * index = new unsigned long();
	unsigned char ret = BitScanMSB(index, 0);
	ASSERT_EQ (ret, 0);
	delete index;
}
// --------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST (MacrosHellTest, CountLeadingZerosTest) {
	ASSERT_DEATH (CountLeadingZeros(0), "(.*?)");
	ASSERT_EQ (CountLeadingZeros(1), 63);
	ASSERT_EQ (CountLeadingZeros(2), 62);
	ASSERT_EQ (CountLeadingZeros(3), 62);
	ASSERT_EQ (CountLeadingZeros(0x8000000000000000ULL), 0);
}

TEST (MacrosHellTest, CountTrailingZerosTest) {
	ASSERT_DEATH (CountTrailingZeros(0), "(.*?)");
	ASSERT_EQ (CountTrailingZeros(1), 0);
	ASSERT_EQ (CountTrailingZeros(2), 1);
	ASSERT_EQ (CountTrailingZeros(3), 0);
	ASSERT_EQ (CountTrailingZeros(0x8000000000000000ULL), 63);
}
// --------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST (MacrosHellTest, GetLSBTest) {
	ASSERT_EQ (GetLSB(0), 0);
	ASSERT_EQ (GetLSB(1), 1);
	ASSERT_EQ (GetLSB(2), 2);
	ASSERT_EQ (GetLSB(3), 1);
	ASSERT_EQ (GetLSB(0x8000000000000000ULL), 0x8000000000000000ULL);
}

TEST (MacrosHellTest, GetMSBTest) {
	ASSERT_EQ (GetMSB(0), 0);
	ASSERT_EQ (GetMSB(1), 1);
	ASSERT_EQ (GetMSB(2), 2);
	ASSERT_EQ (GetMSB(3), 2);
	ASSERT_EQ (GetMSB(0x8000000000000000ULL), 0x8000000000000000ULL);
}

TEST (MacrosHellTest, RemoveLSBTest) {
	uint64_t a;
	a = 0; RemoveLSB(a); ASSERT_EQ (a, 0);
	a = 1; RemoveLSB(a); ASSERT_EQ (a, 0);
	a = 2; RemoveLSB(a); ASSERT_EQ (a, 0);
	a = 3; RemoveLSB(a); ASSERT_EQ (a, 2);
	a = 0x8000000001000000ULL; RemoveLSB(a); ASSERT_EQ (a, 0x8000000000000000ULL);
}

TEST (MacrosHellTest, RemoveMSBTest) {
	uint64_t a;
	a = 0; RemoveMSB(a); ASSERT_EQ (a, 0);
	a = 1; RemoveMSB(a); ASSERT_EQ (a, 0);
	a = 2; RemoveMSB(a); ASSERT_EQ (a, 0);
	a = 3; RemoveMSB(a); ASSERT_EQ (a, 1);
	a = 0x8000000001000000ULL; RemoveMSB(a); ASSERT_EQ (a, 0x0000000001000000ULL);
}
// --------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST (MacrosHellTest, SetBitTest) {
	for (uint8_t i = 0; i < 64; i++)
	{
		uint64_t a = 0ULL;
		SetBit(a, i);
		ASSERT_EQ (a, 1ULL << i);
	}
	{
		uint64_t a = 0ULL;
		ASSERT_DEATH (SetBit(a, 64);, "(.*?)");
	}
}

TEST (MacrosHellTest, GetBitTest) {
	uint64_t a = 0x8000000000000008ULL;
	ASSERT_EQ (GetBit(a,  0), true);
	ASSERT_EQ (GetBit(a, 63), true);
	for (uint8_t i = 1; i < 63; i++)
		ASSERT_EQ (GetBit(a, i), false);
	
	ASSERT_DEATH (GetBit(a, 64);, "(.*?)");
}

TEST (MacrosHellTest, MakeBitTest) {
	for (uint8_t i = 0; i < 64; i++)
		ASSERT_EQ (MakeBit(a, i), 1ULL << i);
	ASSERT_EQ (MakeBit(a, 64), 0ULL);
}
// --------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------------
TEST (MacrosHellTest, PopCountTest) {
	ASSERT_EQ (PopCount(0), 0);
	ASSERT_EQ (PopCount(1), 1);
	ASSERT_EQ (PopCount(2), 1);
	ASSERT_EQ (PopCount(3), 2);
	ASSERT_EQ (PopCount(0xFFFFFFFFFFFFFFFFULL), 64);
}

TEST (MacrosHellTest, PopCount_max15Test) {
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
