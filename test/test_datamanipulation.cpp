#include "datamanipulation.h"
#include "gtest/gtest.h"
#include <cstdint>
#include <utility>
#include <vector>

template <typename T>
void TestReadWrite(const std::size_t size)
{
	{
		std::vector<T> vec;
		for (std::size_t i = 0; i < size; i++)
			vec.push_back(static_cast<T>(i));
		write_to_file("tmp.vec~", vec);
	}
	{
		std::vector<T> vec = read_vector<T>("tmp.vec~");
		ASSERT_EQ (vec.size(), static_cast<std::size_t>(size));
		for (std::size_t i = 0; i < size; i++)
			ASSERT_EQ (vec[i], static_cast<T>(i));
	}
}

TEST (Datamanipulation, ReadWrite___int8) { TestReadWrite<  int8_t>(100); }
TEST (Datamanipulation, ReadWrite__uint8) { TestReadWrite< uint8_t>(100); }
TEST (Datamanipulation, ReadWrite__int16) { TestReadWrite< int16_t>(100); }
TEST (Datamanipulation, ReadWrite_uint16) { TestReadWrite<uint16_t>(100); }
TEST (Datamanipulation, ReadWrite__int32) { TestReadWrite< int32_t>(100); }
TEST (Datamanipulation, ReadWrite_uint32) { TestReadWrite<uint32_t>(100); }
TEST (Datamanipulation, ReadWrite__int64) { TestReadWrite< int64_t>(100); }
TEST (Datamanipulation, ReadWrite_uint64) { TestReadWrite<uint64_t>(100); }
TEST (Datamanipulation, ReadWrite_double) { TestReadWrite<  double>(100); }
TEST (Datamanipulation, ReadWrite__float) { TestReadWrite<   float>(100); }

TEST (Datamanipulation, ReadWrite_class)
{
	struct Cclass
	{
		int a; double b; float c;
		Cclass(int a, double b, float c) : a(a), b(b), c(c) {}
		Cclass() : Cclass(0,0,0) {}
		bool operator==(const Cclass& o) const { return (a == o.a) && (b == o.b) && (c == o.c); }
	};
	const std::size_t size = 100;
	{
		std::vector<Cclass> vec;
		for (std::size_t i = 0; i < size; i++)
			vec.push_back(Cclass(i,i,i));
		write_to_file("tmp.vec~", vec);
	}
	{
		std::vector<Cclass> vec = read_vector<Cclass>("tmp.vec~");
		ASSERT_EQ (vec.size(), static_cast<std::size_t>(size));
		for (std::size_t i = 0; i < size; i++)
			ASSERT_EQ (vec[i], Cclass(i,i,i));
	}
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
