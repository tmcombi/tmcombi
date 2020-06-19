#include "gtest/gtest.h"
#include "main/hello-greet.h"
//#define BOOST_TEST_MODULE MyTest
//#include <boost/test/unit_test.hpp>

TEST(HelloTest, GetGreet) {
  EXPECT_EQ(get_greet("Bazel"), "Hello Bazel");
  EXPECT_EQ(2, 2);
  EXPECT_EQ(3, 3);
}

TEST(HelloTest, GetGreet1) {
  EXPECT_EQ(get_greet("Bazel1"), "Hello Bazel1");
  EXPECT_EQ(4, 4);
  EXPECT_EQ(3, 3);
  EXPECT_EQ(3, 2);
}

TEST(HelloTest1, GetGreet1) {
  EXPECT_EQ(get_greet("Bazel1"), "Hello Bazel1");
  EXPECT_EQ(4, 4);
  EXPECT_EQ(3, 3);
}
