#include "pch.h"
#include "Args.h"

TEST(Args, From_main)
{
	char* argv[] = { "moi.exe", "-v", "-f", "file1", "file2", "-w", "w1", "w2" };
	CArgs args(8, argv);

	ASSERT_TRUE(args.Has("v"));
	ASSERT_TRUE(args.Has("f"));
	ASSERT_TRUE(args.Has("w"));

	ASSERT_EQ(args.Get("f").size(), 2);
	ASSERT_EQ(args.Get("f")[0], "file1");
	ASSERT_EQ(args.Get("f")[1], "file2");

	ASSERT_EQ(args.Get("w").size(), 2);
	ASSERT_EQ(args.Get("w")[0], "w1");
	ASSERT_EQ(args.Get("w")[1], "w2");
}

TEST(Args, From_main_duplicates)
{
	char* argv[] = { "moi.exe", "-v", "-f", "file1", "-f", "file2", "-w", "w1", "-w", "w2" };
	CArgs args(10, argv);

	ASSERT_TRUE(args.Has("v"));
	ASSERT_TRUE(args.Has("f"));
	ASSERT_TRUE(args.Has("w"));

	ASSERT_EQ(args.Get("f").size(), 2);
	ASSERT_EQ(args.Get("f")[0], "file1");
	ASSERT_EQ(args.Get("f")[1], "file2");

	ASSERT_EQ(args.Get("w").size(), 2);
	ASSERT_EQ(args.Get("w")[0], "w1");
	ASSERT_EQ(args.Get("w")[1], "w2");
}

TEST(Args, From_defaults)
{
	CArgs args;
	args.Set("config", "config.ini");
	args.Set("RAM", "1GB");
	args.Set("t", 8);
	args.Set("v");

	ASSERT_TRUE(args.Has("config"));
	ASSERT_TRUE(args.Has("RAM"));
	ASSERT_TRUE(args.Has("t"));
	ASSERT_TRUE(args.Has("v"));
	ASSERT_FALSE(args.Has("x"));

	ASSERT_EQ(args.Get("config").back(), "config.ini");
	ASSERT_EQ(args.Get("RAM").back(), "1GB");
	ASSERT_EQ(args.Get("t").back(), "8");
}

TEST(Args, From_main_with_defaults)
{
	CArgs args;
	args.Set("config", "config.ini");
	args.Set("RAM", "1GB");
	args.Set("t", 8);
	args.Set("v");

	char* argv[] = { "moi.exe", "-f", "file" };
	args.Load(3, argv);

	ASSERT_TRUE(args.Has("config"));
	ASSERT_TRUE(args.Has("RAM"));
	ASSERT_TRUE(args.Has("t"));
	ASSERT_TRUE(args.Has("f"));
	ASSERT_TRUE(args.Has("v"));
	ASSERT_FALSE(args.Has("x"));

	ASSERT_EQ(args.Get("config").back(), "config.ini");
	ASSERT_EQ(args.Get("RAM").back(), "1GB");
	ASSERT_EQ(args.Get("t").back(), "8");
	ASSERT_EQ(args.Get("f").back(), "file");
}