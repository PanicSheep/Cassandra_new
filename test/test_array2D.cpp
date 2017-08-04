#include "gtest/gtest.h"
#include "array2D.h"
#include <cstdint>

// Only moveable
class TestClass1
{
	int * m;
public:
	TestClass1() = default; 
	TestClass1(int * m) : m(m) {}
	
	TestClass1(const TestClass1&  o) = delete;
	TestClass1(      TestClass1&& o) { std::swap(m, o.m); }
	
	TestClass1& operator=(const TestClass1&  o) = delete;
	TestClass1& operator=(      TestClass1&& o) { std::swap(m, o.m); return *this; }
	
	inline int Get() const { return *m; }
};

TEST (Array2D, Int)
{
	Array2D<int> arr(2,3);
	
	ASSERT_EQ (arr.n(), 2);
	ASSERT_EQ (arr.m(), 3);
	
	for (std::size_t i = 0; i < arr.n(); i++)
		for (std::size_t j = 0; j < arr.m(); j++)
			arr(i,j) = i + 10 * j;
	
	for (std::size_t i = 0; i < arr.n(); i++)
		for (std::size_t j = 0; j < arr.m(); j++)
			ASSERT_EQ (arr(i,j), i + 10 * j);
}

TEST (Array2D, Pointer)
{
	Array2D<int*> arr(2,3);
	
	ASSERT_EQ (arr.n(), 2);
	ASSERT_EQ (arr.m(), 3);
	
	for (std::size_t i = 0; i < arr.n(); i++)
		for (std::size_t j = 0; j < arr.m(); j++)
			arr(i,j) = new int(i + 10 * j);
	
	for (std::size_t i = 0; i < arr.n(); i++)
		for (std::size_t j = 0; j < arr.m(); j++)
			ASSERT_EQ (*arr(i,j), i + 10 * j);
}

TEST (Array2D, MoveOnly)
{
	Array2D<TestClass1> arr(2,3);
	
	ASSERT_EQ (arr.n(), 2);
	ASSERT_EQ (arr.m(), 3);
	
	for (std::size_t i = 0; i < arr.n(); i++)
		for (std::size_t j = 0; j < arr.m(); j++)
			arr(i,j) = TestClass1(new int(i + 10 * j));
	
	for (std::size_t i = 0; i < arr.n(); i++)
		for (std::size_t j = 0; j < arr.m(); j++)
			ASSERT_EQ (arr(i,j).Get(), i + 10 * j);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);	
	return RUN_ALL_TESTS();
}