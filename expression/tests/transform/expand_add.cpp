#include <gtest/gtest.h>

#include <string>

#include "../../expression.h"

using namespace Spp;
using namespace std;

inline string& remove_whitespace(string& s) {
  s.erase(remove(s.begin(), s.end(), ' '), s.end());
  return s;
}

TEST(ExprTransformTest, ExpandAddTest) {
  {
    auto a = Expression{1} + Expression{1};
    auto b = a * a;
    auto x = b.to_string();
    x = remove_whitespace(x);
    EXPECT_EQ(x, "(1+1)*(1+1)");
    a = b.expand_add();
    x = a.to_string();
    x = remove_whitespace(x);
    EXPECT_EQ(x, "1*1+1*1+1*1+1*1");
  }
  {
    auto a = -Expression{1};
    auto b = -Expression{2};
    a = (a + b) * (a - b);  // ((-1) + (-2))*((-1)-(-2));
    a = a.expand_add();
    auto x = a.to_string();
    x = remove_whitespace(x);
    EXPECT_EQ(x, "(-1)*(-1)+(-1)*(--2)+(-2)*(-1)+(-2)*(--2)");
    EXPECT_EQ(a.simplify().to_string(), "-3");
  }
  {
    auto a = Expression{1} - Expression{2};
    auto x = a.to_string();
    x = remove_whitespace(x);
    EXPECT_EQ(x, "1-2");
    a = a.expand_add();
    x = a.to_string();
    x = remove_whitespace(x);
    EXPECT_EQ(x, "1+(-2)");
  }
  {
    auto a = Expression{1} - (Expression{2} - Expression{3});  // 1 - (2 - 3)
    a = a.expand_add();  // 1 + (-2) + (--3)
    auto x = a.to_string();
    x = remove_whitespace(x);
    EXPECT_EQ(x, "1+(-2)+(--3)");
    EXPECT_EQ(a.simplify().to_string(), "2");
  }
  {
    auto a = Expression{1} - Expression{2};
    auto b = Expression{1} - Expression{2};
    a = a - b;           // (1-2)-(1-2)
    a = a.expand_add();  // 1 + (-2) + (-1) + (--2)
    auto x = a.to_string();
    x = remove_whitespace(x);
    EXPECT_EQ(x, "1+(-2)+(-1)+(--2)");
    EXPECT_EQ(a.simplify().to_string(), "0");
  }
}
