#include "gtest/gtest.h"
#include "vecvec.h"
#include "vector_extensions.h"
#include <cstdint>
#include <vector>

// No default constructor only moveable
class TestClass1
{
	int * m;
public:
	TestClass1() = delete;
	TestClass1(int * m) : m(m) {}
	
	TestClass1(const TestClass1&  o) = delete;
	TestClass1(      TestClass1&& o) { std::swap(m, o.m); }
	
	TestClass1& operator=(const TestClass1&  o) = delete;
	TestClass1& operator=(      TestClass1&& o) { std::swap(m, o.m); return *this; }
	
	inline bool operator==(const TestClass1& o) const { return m == o.m; }
	inline bool operator!=(const TestClass1& o) const { return m != o.m; }
	
	inline int Get() const { return *m; }
};

TEST (Vecvec, Int)
{
	Vecvec<int> vec(3);
	
	ASSERT_EQ (vec.n(), 3u);
	
	for (std::size_t i = 0; i < vec.n(); i++)
		vec(i) = std::vector<int>(i,i);
	
	for (std::size_t i = 0; i < vec.n(); i++)
		ASSERT_EQ (vec.ns()[i], i); 
	
	for (std::size_t i = 0; i < vec.n(); i++)
		ASSERT_EQ (vec(i), std::vector<int>(i,i));
}


TEST (Vecvec, MoveOnly)
{
	Vecvec<TestClass1> vec(3);
	
	ASSERT_EQ (vec.n(), 3u);
	
	std::vector<TestClass1> tmp1;
	tmp1.push_back(TestClass1(new int(1)));
	vec(0) = std::move(tmp1);
	
	std::vector<TestClass1> tmp2;
	tmp2.push_back(TestClass1(new int(2)));
	tmp2.push_back(TestClass1(new int(3)));
	vec(1) = std::move(tmp2);
	
	std::vector<TestClass1> tmp3;
	tmp3.push_back(TestClass1(new int(4)));
	tmp3.push_back(TestClass1(new int(5)));
	tmp3.push_back(TestClass1(new int(6)));
	vec(2) = std::move(tmp3);
	
	
	for (std::size_t i = 0; i < vec.n(); i++)
		ASSERT_EQ (vec.ns()[i], i+1);
	
	ASSERT_EQ (vec(0)[0].Get(), 1);
	
	ASSERT_EQ (vec(1)[0].Get(), 2);
	ASSERT_EQ (vec(1)[1].Get(), 3);
	
	ASSERT_EQ (vec(2)[0].Get(), 4);
	ASSERT_EQ (vec(2)[1].Get(), 5);
	ASSERT_EQ (vec(2)[2].Get(), 6);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);	
	return RUN_ALL_TESTS();
}