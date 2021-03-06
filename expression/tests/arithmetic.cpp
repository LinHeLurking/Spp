#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>

#include "../expression.h"
#include "util/common.h"

using namespace Spp;
using namespace std;

TEST(ExprTest, NegTest) {
  Expression a{1};
  string x = (-a).to_string();
  x = remove_whitespace(x);
  EXPECT_EQ(x, "-1");

  x = (-std::move(a)).to_string();
  x = remove_whitespace(x);
  EXPECT_EQ(x, "-1");
}
#define TEST_BIN_OP_SINGLE(op, i, j)       \
  {                                        \
    Expression a{i};                       \
    Expression b{j};                       \
    auto c = a op b;                       \
    auto x = c.to_string();                \
    x = remove_whitespace(x);              \
    stringstream ss;                       \
    ss << i << #op << j;                   \
    EXPECT_EQ(ss.str(), x);                \
  }                                        \
  {                                        \
    Expression a{i};                       \
    Expression b{j};                       \
    auto c = a op std::move(b);            \
    auto x = c.to_string();                \
    x = remove_whitespace(x);              \
    stringstream ss;                       \
    ss << i << #op << j;                   \
    EXPECT_EQ(ss.str(), x);                \
  }                                        \
  {                                        \
    Expression a{i};                       \
    Expression b{j};                       \
    auto c = std::move(a) op b;            \
    auto x = c.to_string();                \
    x = remove_whitespace(x);              \
    stringstream ss;                       \
    ss << i << #op << j;                   \
    EXPECT_EQ(ss.str(), x);                \
  }                                        \
  {                                        \
    Expression a{i};                       \
    Expression b{j};                       \
    auto c = std::move(a) op std::move(b); \
    auto x = c.to_string();                \
    x = remove_whitespace(x);              \
    stringstream ss;                       \
    ss << i << #op << j;                   \
    EXPECT_EQ(ss.str(), x);                \
  }

#define TEST_BIN_OP(op)             \
  for (int i = 1; i < 100; ++i) {   \
    for (int j = 1; j < 100; ++j) { \
      TEST_BIN_OP_SINGLE(op, i, j); \
    }                               \
  }

#define TEST_BIN_EVAL_SINGLE(op, i, j) \
  {                                    \
    Expression a{i};                   \
    Expression b{j};                   \
    auto c = (a op b);                 \
    c = c.simplify();                  \
    auto x = c.to_string();            \
    x = remove_whitespace(x);          \
    EXPECT_EQ(x, to_string(i op j));   \
  }

#define TEST_BIN_EVAL(op)             \
  for (int i = 0; i < 100; ++i) {     \
    for (int j = 0; j < 100; ++j) {   \
      TEST_BIN_EVAL_SINGLE(op, i, j); \
    }                                 \
  }

TEST(ExprArithmeticTest, AddTest) {
  TEST_BIN_OP(+);
  TEST_BIN_EVAL(+);
}
TEST(ExprArithmeticTest, SubTest) {
  TEST_BIN_OP(-);
  TEST_BIN_EVAL(-);
}
TEST(ExprArithmeticTest, MulTest) {
  TEST_BIN_OP(*);
  TEST_BIN_EVAL(*);
}
TEST(ExprArithmeticTest, DivTest) { TEST_BIN_OP(/); }
