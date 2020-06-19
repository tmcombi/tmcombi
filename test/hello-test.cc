#include "gtest/gtest.h"
#include "main/hello-greet.h"

TEST(HelloTest, GetGreet) {
  EXPECT_EQ(get_greet("Bazel"), "Hello Bazel");
  EXPECT_EQ(2, 2);
  EXPECT_EQ(3, 3);
}

TEST(HelloTest, GetGreet1) {
  EXPECT_EQ(get_greet("Bazel1"), "Hello Bazel1");
  EXPECT_EQ(4, 4);
  EXPECT_EQ(3, 3);
}

TEST(HelloTest1, GetGreet1) {
  EXPECT_EQ(get_greet("Bazel1"), "Hello Bazel1");
  EXPECT_EQ(4, 4);
  EXPECT_EQ(3, 3);
}
