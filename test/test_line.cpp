#include "line.h"
#include "gtest/gtest.h"

TEST (LineTest, UseCase1) {
	CLine a(1); a.NewPV(12);
	CLine b(2); b.NewPV(13, a);
	CLine c(3); c.NewPV(14, b);
	
	ASSERT_EQ (c.GetPV(), "E2 F2 G2");
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
