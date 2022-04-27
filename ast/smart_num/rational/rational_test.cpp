#include "rational.h"
#include <gtest/gtest.h>

using namespace Spp::__SmartNum::__Detail;

TEST(RationalTest, EqTest) {
  // This will fail to compile if everything is coded right.
  // auto err = Rational<int>(1, 3);

  auto a = Rational<uint>(1, 2);
  auto b = Rational<uint>(2, 4);

  EXPECT_TRUE(a == b);

  auto c = Rational<>(3, 15);
  auto d = Rational<>(1, 5);
  EXPECT_EQ(c, d);

  auto e = Rational<>(0, 2);
  auto f = Rational<>(0, 3);
  EXPECT_EQ(e, f);
  EXPECT_EQ(e, 0);
  EXPECT_EQ(f, 0);

  auto g = Rational<uint>(1, 2, -1);
  auto h = Rational<uint64_t>(1, 2, -1);
  EXPECT_EQ(g, h);

  auto i = Rational<>(1, 1, -1);
  auto j = Rational<>(2, 2, -1);
  EXPECT_EQ(i, j);
  EXPECT_EQ(i, -1);
  EXPECT_EQ(-1, i);
  EXPECT_EQ(j, -1LL);
  EXPECT_EQ(-1LL, j);

  auto k = Rational<>(1, 1);
  auto l = Rational<>(2, 2);
  EXPECT_EQ(k, l);
  EXPECT_EQ(k, 1U);
  EXPECT_EQ(1U, k);
  EXPECT_EQ(l, 1ULL);
  EXPECT_EQ(1ULL, l);
}

TEST(RationalTest, AddTest) {
  auto a = Rational<>(1, 2);
  auto b = Rational<>(1, 3);
  auto c = Rational<>(5, 6);

  EXPECT_EQ(a + b, c);

  auto d = Rational<>(1, 2, -1);
  auto e = Rational<>(1, 2);
  EXPECT_EQ(d + e, 0);
  EXPECT_EQ(e + d, 0);
  EXPECT_EQ(d + 1, e);
  EXPECT_EQ(1 + d, e);
}

TEST(RationalTest, SubTest) {
  auto a = Rational<>(1, 2);
  auto b = Rational<>(1, 4);
  EXPECT_EQ(a - 1, -a);
  EXPECT_EQ(a + b, 1 - b);
  EXPECT_EQ(a - b, b);
  EXPECT_EQ(b - a, -b);
  EXPECT_EQ(b - a, 0 - b);

  auto c = Rational<>(1, 3, -1);
  auto d = Rational<>(1, 2, -1);
  auto e = Rational<>(1, 6, -1);
  EXPECT_EQ(d - c, e);
  EXPECT_EQ(c - d, -e);
  EXPECT_EQ(c - d, 0 - e);
}

TEST(RationalTest, MulTest) {
  auto a = Rational<>(1, 2);
  auto b = Rational<>(1, 4);
  EXPECT_EQ(a * a, b);
  EXPECT_EQ(b * 2, a);
  EXPECT_EQ(2 * b, a);

  auto c = Rational<>(1, 6, -1);
  auto d = Rational<>(2, 3, -1);
  auto e = Rational<>(1, 9);
  EXPECT_EQ(c * d, e);
}

TEST(RationalTest, DivTest) {
  auto a = Rational<>(1, 2);
  auto b = Rational<>(1, 4, -1);
  EXPECT_EQ(a / b, -2);
  EXPECT_EQ(b / a, -a);
}

TEST(RationalTest, CastTest) {
  auto a = double(Rational<>(1, 2));
  auto b = 0.5;
  EXPECT_NEAR(a, b, 1e-6);
}
