#include <gtest/gtest.h>

#include "../../expression.h"
#include "../util/common.h"
using namespace Spp;

TEST(ExprTransformTest, CollectTest) {
  Expression a{"x"}, b{"1"}, c{"x"};
  auto d = a + b + c;
  d = d.expand_add().collect();
  auto x = d.to_string();
  x = remove_whitespace(x);
  // For now, reorder function is not implemented.
  // So compare all possible results.
  EXPECT_TRUE(x == "2*x+1" || x == "1+2*x" || x == "x*2+1" || x == "1+x*2")
      << x << std::endl;
}
