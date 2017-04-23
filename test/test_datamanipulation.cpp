#include "datamanipulation.h"
#include "gtest/gtest.h"
#include <cstdint>
#include <utility>
#include <vector>
#include <set>

template <typename T>
void TestReadWriteVec(const std::size_t size)
{
	{
		std::vector<T> vec;
		for (std::size_t i = 0; i < size; i++)
			vec.push_back(static_cast<T>(i));
		write_to_file("tmp.vec~", vec);
	}
	{
		std::vector<T> vec = read_vector<T>("tmp.vec~");
		ASSERT_EQ (vec.size(), size);
		for (std::size_t i = 0; i < size; i++)
			ASSERT_EQ (vec[i], static_cast<T>(i));
	}
}

TEST (Datamanipulation, ReadWriteVec___int8) { TestReadWriteVec<  int8_t>(100); }
TEST (Datamanipulation, ReadWriteVec__uint8) { TestReadWriteVec< uint8_t>(100); }
TEST (Datamanipulation, ReadWriteVec__int16) { TestReadWriteVec< int16_t>(100); }
TEST (Datamanipulation, ReadWriteVec_uint16) { TestReadWriteVec<uint16_t>(100); }
TEST (Datamanipulation, ReadWriteVec__int32) { TestReadWriteVec< int32_t>(100); }
TEST (Datamanipulation, ReadWriteVec_uint32) { TestReadWriteVec<uint32_t>(100); }
TEST (Datamanipulation, ReadWriteVec__int64) { TestReadWriteVec< int64_t>(100); }
TEST (Datamanipulation, ReadWriteVec_uint64) { TestReadWriteVec<uint64_t>(100); }
TEST (Datamanipulation, ReadWriteVec_double) { TestReadWriteVec<  double>(100); }
TEST (Datamanipulation, ReadWriteVec__float) { TestReadWriteVec<   float>(100); }

TEST (Datamanipulation, ReadWriteVec_class)
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

template <typename T>
void TestReadWriteIteratorVec(const std::size_t size)
{
	{
		std::vector<T> vec;
		for (std::size_t i = 0; i < size; i++)
			vec.push_back(static_cast<T>(i));
		write_to_file("tmp.vec~", vec.begin(), vec.end());
	}
	{
		std::vector<T> vec = read_vector<T>("tmp.vec~");
		ASSERT_EQ (vec.size(), size);
		for (std::size_t i = 0; i < size; i++)
			ASSERT_EQ (vec[i], static_cast<T>(i));
	}
}

TEST (Datamanipulation, ReadWriteIteratorVec___int8) { TestReadWriteIteratorVec<  int8_t>(100); }
TEST (Datamanipulation, ReadWriteIteratorVec__uint8) { TestReadWriteIteratorVec< uint8_t>(100); }
TEST (Datamanipulation, ReadWriteIteratorVec__int16) { TestReadWriteIteratorVec< int16_t>(100); }
TEST (Datamanipulation, ReadWriteIteratorVec_uint16) { TestReadWriteIteratorVec<uint16_t>(100); }
TEST (Datamanipulation, ReadWriteIteratorVec__int32) { TestReadWriteIteratorVec< int32_t>(100); }
TEST (Datamanipulation, ReadWriteIteratorVec_uint32) { TestReadWriteIteratorVec<uint32_t>(100); }
TEST (Datamanipulation, ReadWriteIteratorVec__int64) { TestReadWriteIteratorVec< int64_t>(100); }
TEST (Datamanipulation, ReadWriteIteratorVec_uint64) { TestReadWriteIteratorVec<uint64_t>(100); }
TEST (Datamanipulation, ReadWriteIteratorVec_double) { TestReadWriteIteratorVec<  double>(100); }
TEST (Datamanipulation, ReadWriteIteratorVec__float) { TestReadWriteIteratorVec<   float>(100); }

TEST (Datamanipulation, ReadWriteIteratorVec_class)
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
		write_to_file("tmp.vec~", vec.begin(), vec.end());
	}
	{
		std::vector<Cclass> vec = read_vector<Cclass>("tmp.vec~");
		ASSERT_EQ (vec.size(), static_cast<std::size_t>(size));
		for (std::size_t i = 0; i < size; i++)
			ASSERT_EQ (vec[i], Cclass(i,i,i));
	}
}

template <typename T>
void TestReadWriteIteratorSet(const std::size_t size)
{
	{
		std::set<T> set;
		for (std::size_t i = 0; i < size; i++)
			set.insert(static_cast<T>(i));
		write_to_file("tmp.vec~", set.begin(), set.end());
	}
	{
		std::vector<T> vec = read_vector<T>("tmp.vec~");
		ASSERT_EQ (vec.size(), size);
		for (std::size_t i = 0; i < size; i++)
			ASSERT_EQ (vec[i], static_cast<T>(i));
	}
}

TEST (Datamanipulation, ReadWriteIteratorSet___int8) { TestReadWriteIteratorSet<  int8_t>(100); }
TEST (Datamanipulation, ReadWriteIteratorSet__uint8) { TestReadWriteIteratorSet< uint8_t>(100); }
TEST (Datamanipulation, ReadWriteIteratorSet__int16) { TestReadWriteIteratorSet< int16_t>(100); }
TEST (Datamanipulation, ReadWriteIteratorSet_uint16) { TestReadWriteIteratorSet<uint16_t>(100); }
TEST (Datamanipulation, ReadWriteIteratorSet__int32) { TestReadWriteIteratorSet< int32_t>(100); }
TEST (Datamanipulation, ReadWriteIteratorSet_uint32) { TestReadWriteIteratorSet<uint32_t>(100); }
TEST (Datamanipulation, ReadWriteIteratorSet__int64) { TestReadWriteIteratorSet< int64_t>(100); }
TEST (Datamanipulation, ReadWriteIteratorSet_uint64) { TestReadWriteIteratorSet<uint64_t>(100); }
TEST (Datamanipulation, ReadWriteIteratorSet_double) { TestReadWriteIteratorSet<  double>(100); }
TEST (Datamanipulation, ReadWriteIteratorSet__float) { TestReadWriteIteratorSet<   float>(100); }

TEST (Datamanipulation, ReadWriteIteratorSet_class)
{
	struct Cclass
	{
		int a; double b; float c;
		Cclass(int a, double b, float c) : a(a), b(b), c(c) {}
		Cclass() : Cclass(0,0,0) {}
		bool operator==(const Cclass& o) const { return (a == o.a) && (b == o.b) && (c == o.c); }
		bool operator< (const Cclass& o) const { return (a < o.a) || ((a == o.a) && (b < o.b))  || ((a == o.a) && (b == o.b) && (c < o.c)); }
	};
	const std::size_t size = 100;
	{
		std::set<Cclass> set;
		for (std::size_t i = 0; i < size; i++)
			set.insert(Cclass(i,i,i));
		write_to_file("tmp.vec~", set.begin(), set.end());
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
