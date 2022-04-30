#ifndef SPP_SMART_NUM_H
#define SPP_SMART_NUM_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <numbers>
#include <tuple>
#include <type_traits>
#include <variant>

#include "../../../util/concept.h"
#include "rational/rational.h"

namespace Spp::__SmartNum {
using Spp::__Concept::SignedInteger;
class SmartNum {
 private:
  using Rational = __Detail::Rational<>;
  using Value = std::variant<int64_t, Rational, double>;
  Value value_;

  template <typename T>
  static inline std::pair<Value, Value> shift(const Value &a, const Value &b) {
    auto x = std::visit([](auto &&arg) { return T(arg); }, a);
    auto y = std::visit([](auto &&arg) { return T(arg); }, b);
    return {x, y};
  }

  static inline std::pair<Value, Value> shift(const Value &a, const Value &b) {
    int idx = std::max(a.index(), b.index());
    if (idx == 0) {
      return shift<int64_t>(a, b);
    } else if (idx == 1) {
      return shift<Rational>(a, b);
    } else {  // idx == 2
      return shift<double>(a, b);
    }
  }

 public:
  SmartNum() = default;

  template <typename T>
  requires std::is_integral_v<T>
  explicit SmartNum(T n) : value_(int64_t(n)) {}

  template <typename T>
  requires std::is_floating_point_v<T>
  explicit SmartNum(T x) : value_(double(x)) {}

  template <typename T, typename U>
  requires std::is_integral_v<T> && std::is_integral_v<U> SmartNum(
      T nominator, U denominator, int sign = 1) {
    using R =
        std::common_type_t<std::make_unsigned_t<T>, std::make_unsigned_t<U>>;
    R n;
    R d;
    if (nominator < 0) {
      sign = -sign;
      n = -nominator;
    } else {
      n = nominator;
    }
    if (denominator < 0) {
      sign = -sign;
      denominator = -denominator;
    } else {
      d = denominator;
    }
    value_ = Rational(n, d, sign);
  }

  SmartNum(const Rational &r) : value_(r) {}

  static inline auto one() { return SmartNum(1); };

  static inline auto zero() { return SmartNum(0); };

  static inline auto pi() { return SmartNum(std::numbers::pi); }

  inline void cast_trivial_rational() {
    if (value_.index() == 1) {
      auto &inner = std::get<1>(value_);
      if (inner.denominator_ == 1) {
        value_ = inner.denominator_;
      }
    }
  }

  inline uint64_t hash_code() const {
    return std::visit(
        [](Value &&arg) -> uint64_t {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, int64_t>) {
            return std::get<int64_t>(arg);
          } else if constexpr (std::is_same_v<T, Rational>) {
            auto &x = std::get<Rational>(arg);
            return ((x.nominator_) ^ (x.denominator_ << 1)) << 1;
          } else /*if constexpr (std::is_same_v<T, double>) */ {
            double x = std::get<double>(arg);
            return (*(uint64_t *)(&x)) << 1 | 1;
          }
        },
        value_);
  }

  inline bool operator==(const SmartNum &rhs) const {
    auto [x, y] = shift(value_, rhs.value_);
    return std::visit([](auto &&l, auto &&r) { return l == r; }, x, y);
  }

  inline SmartNum operator-() const {
    return std::visit([](auto &&arg) { return SmartNum(-arg); }, value_);
  }

  inline SmartNum operator+(const SmartNum &rhs) const {
    return std::visit([](auto &&l, auto &&r) { return SmartNum(l + r); },
                      value_, rhs.value_);
  }

  inline SmartNum operator-(const SmartNum &rhs) const {
    return std::visit([](auto &&l, auto &&r) { return SmartNum(l - r); },
                      value_, rhs.value_);
  }

  inline SmartNum operator*(const SmartNum &rhs) const {
    return std::visit([](auto &&l, auto &&r) { return SmartNum(l * r); },
                      value_, rhs.value_);
  }

  inline SmartNum operator/(const SmartNum &rhs) const {
    return std::visit([](auto &&l, auto &&r) { return SmartNum(l / r); },
                      value_, rhs.value_);
  }

  /**
   * User-defined cast.
   */
  inline operator double() const {
    return std::visit([](auto &&arg) { return double(arg); }, value_);
  }

  /**
   * I/O overload.
   */
  friend inline std::ostream &operator<<(std::ostream &os, const SmartNum &v) {
    std::visit([&](auto &&arg) { os << arg; }, v.value_);
    return os;
  }
};
}  // namespace Spp::__SmartNum

#endif  // !SMART_NUM_H
