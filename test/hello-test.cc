#include "gtest/gtest.h"
#include "main/hello-greet.h"
//#define BOOST_TEST_MODULE MyTest
//#include <boost/test/unit_test.hpp>

TEST(HelloTest, GetGreet) {
  EXPECT_EQ(get_greet("Bazel"), "Hello Bazel");
  EXPECT_EQ(2, 2);
  EXPECT_EQ(3, 3);
}
