#include <gtest/gtest.h>

#include "../../expression.h"
#include "../util/common.h"
using namespace Spp;

TEST(ExprTransformTest, CollectTest) {
  {
    Expression a{"x"}, b{1}, c{"x"};
    auto d = a + b + c;
    d = d.expand_add().collect().reorder();
    auto x = d.to_string();
    x = remove_whitespace(x);
    EXPECT_EQ(x, "1+2*x");
  }
  {
    Expression a{"x"}, b{"y"}, c{1};
    auto d = a + c + b + a + b;  // x + 1 + y + x + y
    d = d.expand_add().collect().reorder();
    auto x = d.to_string();
    x = remove_whitespace(x);
    EXPECT_EQ(x, "1+2*x+2*y");
  }
}
