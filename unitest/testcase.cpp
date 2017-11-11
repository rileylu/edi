#include "stdafx.h"
#include <gtest/gtest.h>

extern int Add(int a, int b);

TEST(testcase, test0)
{
	EXPECT_EQ(14, Add(4, 10));
}

TEST(testcase, test1)
{
	EXPECT_EQ(6, Add(5, 7));
}

TEST(testcase, test2)
{
	EXPECT_EQ(28, Add(10, 18));
}

TEST(testcase, test3)
{
}