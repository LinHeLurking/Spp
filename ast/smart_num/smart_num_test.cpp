#include "smart_num.h"
#include <gtest/gtest.h>
#include <vector>

using Spp::__SmartNum::SmartNum;
using std::vector;

TEST(SmartNumTest, EqTest) {
  SmartNum a(2, 2, -1);
  SmartNum b(-1);
  EXPECT_EQ(a, b);

  SmartNum c(1, 2);
  SmartNum d(0.5);
  EXPECT_EQ(c, d);
  EXPECT_NEAR(c, d, 1e-6);
}

TEST(SmartNumTest, NegTest) {
  SmartNum a(1);
  SmartNum b(-1);
  EXPECT_EQ(a, -b);
  EXPECT_EQ(-a, b);

  SmartNum c(1, 2);
  SmartNum d(1, 2, -1);
  EXPECT_EQ(c, -d);
  EXPECT_EQ(-c, d);
}

#define TEST_BIN_OP_IR(op)                                                     \
  {                                                                            \
    vector<int> x;                                                             \
    for (int i = 0; i < 100; ++i) {                                            \
      x.push_back(i + 1);                                                      \
    }                                                                          \
    for (int i = 0; i < x.size(); ++i) {                                       \
      double a = x[i];                                                         \
      SmartNum l(x[i]);                                                        \
      for (int j = 0; j < x.size(); ++j) {                                     \
        for (int k = 0; k < x.size(); ++k) {                                   \
          double b = double(x[j]) / double(x[k]);                              \
          SmartNum r(x[j], x[k]);                                              \
          EXPECT_NEAR(l op r, a op b, 1e-6)                                    \
              << "l: " << l << ", r:" << r << std::endl                        \
              << "a: " << a << ", b:" << b << std::endl;                       \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  }

#define TEST_BIN_OP_RD(op)                                                     \
  {                                                                            \
    vector<int> x;                                                             \
    for (int i = 0; i < 100; ++i) {                                            \
      x.push_back(i + 1);                                                      \
    }                                                                          \
    for (int i = 0; i < x.size(); ++i) {                                       \
      double a = x[i];                                                         \
      SmartNum l(a);                                                           \
      for (int j = 0; j < x.size(); ++j) {                                     \
        for (int k = 0; k < x.size(); ++k) {                                   \
          double b = double(x[j]) / double(x[k]);                              \
          SmartNum r(x[j], x[k]);                                              \
          EXPECT_NEAR(l op r, a op b, 1e-6)                                    \
              << "l: " << l << ", r:" << r << std::endl                        \
              << "a: " << a << ", b:" << b << std::endl;                       \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  }

TEST(SmartNumTest, AddTest) {
  TEST_BIN_OP_IR(+);
  TEST_BIN_OP_RD(+);
}

TEST(SmartNumTest, SubTest) {
  TEST_BIN_OP_IR(-);
  TEST_BIN_OP_RD(-);
}

TEST(SmartNumTest, MulTest) {
  TEST_BIN_OP_IR(*);
  TEST_BIN_OP_RD(*);
}

TEST(SmartNumTest, DivTest) {
  TEST_BIN_OP_IR(/);
  TEST_BIN_OP_RD(/);
}
