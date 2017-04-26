#include "line.h"
#include "gtest/gtest.h"

TEST (LineTest, UseCase1)
{
	CLine* a = new CLine(1); a->NewPV(12);
	CLine* b = new CLine(2); b->NewPV(13, a);
	CLine* c = new CLine(3); c->NewPV(14, b);
	
	ASSERT_EQ (a->GetPV(), "E2");
	ASSERT_EQ (b->GetPV(), "F2 E2");
	ASSERT_EQ (c->GetPV(), "G2 F2 E2");
	
	delete a;
	delete b;
	delete c;
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
