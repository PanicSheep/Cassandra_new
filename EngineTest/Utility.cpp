#include "pch.h"
#include "Utility.h"

TEST(UtilityTest, RoundInt)
{
	ASSERT_EQ(RoundInt(-1.1f), -1);
	ASSERT_EQ(RoundInt(-1.0f), -1);
	ASSERT_EQ(RoundInt(-0.9f), -1);
	ASSERT_EQ(RoundInt(-0.8f), -1);
	ASSERT_EQ(RoundInt(-0.7f), -1);
	ASSERT_EQ(RoundInt(-0.6f), -1);
	ASSERT_EQ(RoundInt(-0.5f), -1);
	ASSERT_EQ(RoundInt(-0.4f),  0);
	ASSERT_EQ(RoundInt(-0.3f),  0);
	ASSERT_EQ(RoundInt(-0.2f),  0);
	ASSERT_EQ(RoundInt(-0.1f),  0);
	ASSERT_EQ(RoundInt( 0.0f),  0);
	ASSERT_EQ(RoundInt( 0.1f),  0);
	ASSERT_EQ(RoundInt( 0.2f),  0);
	ASSERT_EQ(RoundInt( 0.3f),  0);
	ASSERT_EQ(RoundInt( 0.4f),  0);
	ASSERT_EQ(RoundInt( 0.5f),  1);
	ASSERT_EQ(RoundInt( 0.6f),  1);
	ASSERT_EQ(RoundInt( 0.7f),  1);
	ASSERT_EQ(RoundInt( 0.8f),  1);
	ASSERT_EQ(RoundInt( 0.9f),  1);
	ASSERT_EQ(RoundInt( 1.0f),  1);
	ASSERT_EQ(RoundInt( 1.1f),  1);
}

TEST (UtilityTest, time_format)
{
	//ddd:hh:mm:ss.ccc
	ASSERT_EQ (time_format(std::chrono::milliseconds( 0)), "           0.000");
	ASSERT_EQ (time_format(std::chrono::milliseconds( 1)), "           0.001");
	ASSERT_EQ (time_format(std::chrono::     seconds( 1)), "           1.000");
	ASSERT_EQ (time_format(std::chrono::     minutes( 1)), "        1:00.000");
	ASSERT_EQ (time_format(std::chrono::       hours( 1)), "     1:00:00.000");
	ASSERT_EQ (time_format(std::chrono::       hours(24)), "  1:00:00:00.000");

	auto time = std::chrono::hours(26) + std::chrono::minutes(41) + std::chrono::milliseconds(1);
	ASSERT_EQ (time_format(time), "  1:02:41:00.001");
}

TEST(UtilityTest, ThousandsSeparator)
{
	ASSERT_EQ (ThousandsSeparator(      0),         "0");
	ASSERT_EQ (ThousandsSeparator(      1),         "1");
	ASSERT_EQ (ThousandsSeparator(     10),        "10");
	ASSERT_EQ (ThousandsSeparator(    100),       "100");
	ASSERT_EQ (ThousandsSeparator(   1000),     "1'000");
	ASSERT_EQ (ThousandsSeparator(  10000),    "10'000");
	ASSERT_EQ (ThousandsSeparator( 100000),   "100'000");
	ASSERT_EQ (ThousandsSeparator(1000000), "1'000'000");
}

TEST(UtilityTest, replace_all)
{
	std::string source = "aababba";
	replace_all(source, "b", "c");
	ASSERT_EQ(source, "aacacca");
}

TEST(UtilityTest, replace_all_empty)
{
	std::string source = "";
	replace_all(source, "b", "");
	ASSERT_EQ(source, "");
}

TEST(UtilityTest, replace_all_withEmpty)
{
	std::string source = "aababba";
	replace_all(source, "b", "");
	ASSERT_EQ(source, "aaaa");
}

TEST(UtilityTest, replace_all_none)
{
	std::string source = "aababba";
	replace_all(source, "c", "b");
	ASSERT_EQ(source, "aababba");
}

TEST(UtilityTest, split_simple)
{
	const std::string source = "a,b,c";
	const std::string delimitter = ",";

	std::vector<std::string> vec = split(source, delimitter);

	ASSERT_EQ(vec.size(), 3u);
	ASSERT_EQ(vec[0], "a");
	ASSERT_EQ(vec[1], "b");
	ASSERT_EQ(vec[2], "c");
}

TEST(UtilityTest, split_empty)
{
	const std::string source = "";
	const std::string delimitter = ",";

	std::vector<std::string> vec = split(source, delimitter);

	ASSERT_EQ(vec.size(), 1u);
	ASSERT_EQ(vec[0], "");
}

TEST(UtilityTest, split_none)
{
	const std::string source = "a;b";
	const std::string delimitter = ",";

	std::vector<std::string> vec = split(source, delimitter);

	ASSERT_EQ(vec.size(), 1u);
	ASSERT_EQ(vec[0], "a;b");
}

TEST(UtilityTest, split_one)
{
	const std::string source = "a";
	const std::string delimitter = ",";

	std::vector<std::string> vec = split(source, delimitter);

	ASSERT_EQ(vec.size(), 1u);
	ASSERT_EQ(vec[0], "a");
}

TEST(UtilityTest, split_emptyToken)
{
	const std::string source = "a,,c,";
	const std::string delimitter = ",";

	std::vector<std::string> vec = split(source, delimitter);

	ASSERT_EQ(vec.size(), 4u);
	ASSERT_EQ(vec[0], "a");
	ASSERT_EQ(vec[1], "");
	ASSERT_EQ(vec[2], "c");
	ASSERT_EQ(vec[3], "");
}

TEST(UtilityTest, join_simple)
{
	const std::vector<std::string> parts = { "a", "b", "c" };
	const std::string delimitter = ",";

	const std::string str = join(parts, delimitter);

	ASSERT_EQ(str, "a,b,c");
}

TEST(UtilityTest, join_empty)
{
	const std::vector<std::string> parts;
	const std::string delimitter = ",";

	const std::string str = join(parts, delimitter);

	ASSERT_EQ(str, "");
}

TEST(UtilityTest, join_one)
{
	const std::vector<std::string> parts = { "a" };
	const std::string delimitter = ",";

	const std::string str = join(parts, delimitter);

	ASSERT_EQ(str, "a");
}

TEST(UtilityTest, join_emptyToken)
{
	const std::vector<std::string> parts = { "a", "b", "c" };
	const std::string delimitter = "";

	const std::string str = join(parts, delimitter);

	ASSERT_EQ(str, "abc");
}

TEST(UtilityTest, join_split)
{
	const std::string source = "a,,c,";
	const std::string delimitter = ",";

	const auto parts = split(source, delimitter);
	const std::string str = join(parts, delimitter);

	ASSERT_EQ(str, source);
}
