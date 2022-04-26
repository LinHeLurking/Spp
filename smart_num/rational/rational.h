#ifndef RATIONAL_H
#define RATIONAL_H

#include "../../util/concept.h"
#include <cstdint>
#include <numeric>
#include <type_traits>

namespace Spp::SmartNumDetail {

using namespace Spp::Concept;

template <typename T, typename U> using CT = std::common_type_t<T, U>;

template <typename T = uint64_t>
requires UnsignedInteger<T>
class Rational {
public:
  Rational(T nominator, T denominator, int sign = 1)
      : nominator_(nominator), denominator_(denominator) {
    sign_ = sign >= 0 ? 1 : -1;
    this->reduce();
  }

  Rational(T k) {
    sign_ = k >= 0 ? 1 : -1;
    nominator_ = k >= 0 ? k : -k;
    denominator_ = 1;
  }

  inline auto operator-() const {
    auto that = *this;
    that.sign_ = -that.sign_;
    return that;
  }

  /**
   * Template member operator functions;
   */

  template <typename U>
  requires std::is_integral_v<U>
  inline bool operator==(const U rhs) const {
    return (nominator_ == 0 && rhs == 0) ||
           (denominator_ == 1 &&
            ((sign_ == 1 && nominator_ == rhs) ||
             (std::is_signed_v<U> && sign_ == -1 && nominator_ == -rhs)));
  }

  template <typename U>
  requires std::is_integral_v<U>
  inline auto operator+(const U rhs) const {
    auto r = make_rational(rhs);
    return *this + r;
  }

  template <typename U>
  requires std::is_integral_v<U>
  friend inline auto operator+(const U lhs, const Rational &rhs) {
    return rhs + lhs;
  }

  template <typename U>
  requires std::is_integral_v<U>
  inline auto operator-(const U rhs) const {
    auto r = make_rational(rhs);
    return *this - r;
  }

  template <typename U>
  requires std::is_integral_v<U>
  friend inline auto operator-(const U lhs, const Rational &rhs) {
    return (-rhs) + lhs;
  }

  template <typename U>
  requires std::is_integral_v<U>
  inline auto operator*(const U rhs) const {
    auto r = make_rational(rhs);
    return (*this) * r;
  }

  template <typename U>
  requires std::is_integral_v<U>
  friend inline auto operator*(const U lhs, const Rational &rhs) {
    auto l = make_rational(lhs);
    return l * rhs;
  }

  template <typename U>
  requires std::is_integral_v<U>
  friend inline auto operator/(const U lhs, const Rational &rhs) {
    auto l = make_rational(lhs);
    return l / rhs;
  }

  /**
   * Teamplate friend function forward declarations.
   * These declarations and definitions are separated
   * to avoid redifination of the same function when instantiating
   * with different typename T.
   */

  template <typename U, typename V>
  friend inline bool operator==(const Rational<U> &lhs, const Rational<V> &rhs);

  template <typename U, typename V>
  friend inline auto operator+(const Rational<U> &lhs, const Rational<V> &rhs);

  template <typename U, typename V>
  friend inline auto operator-(const Rational<U> &lhs, const Rational<V> &rhs);

  template <typename U, typename V>
  friend inline auto operator*(const Rational<U> &lhs, const Rational<V> &rhs);

  template <typename U, typename V>
  friend inline auto operator/(const Rational<U> &lhs, const Rational<V> &rhs);

private:
  template <typename U>
  requires std::is_integral_v<U>
  static inline auto make_rational(U n) {
    return Rational<std::make_unsigned_t<U>>(n);
  }
  void reduce() {
    T g = std::gcd(nominator_, denominator_);
    nominator_ /= g;
    denominator_ /= g;
  }

  int sign_;
  T nominator_, denominator_;
};

template <typename U, typename V>
inline bool operator==(const Rational<U> &lhs, const Rational<V> &rhs) {
  return (lhs.nominator_ == 0 && rhs.nominator_ == 0) ||
         (lhs.sign_ == rhs.sign_ && lhs.nominator_ == rhs.nominator_ &&
          lhs.denominator_ == rhs.denominator_);
}

template <typename U, typename V>
inline auto operator+(const Rational<U> &lhs, const Rational<V> &rhs) {
  using R = std::common_type_t<U, V>;
  using SR = std::make_signed_t<R>;
  auto a = SR(lhs.sign_) * SR(lhs.nominator_) * SR(rhs.denominator_);
  auto b = SR(rhs.sign_) * SR(rhs.nominator_) * SR(lhs.denominator_);
  auto c = lhs.denominator_ * rhs.denominator_;
  a += b;
  int s = 1;
  if (a < 0) {
    s = -1;
    a = -a;
  }
  return Rational<R>(a, c, s);
}

template <typename U, typename V>
inline auto operator-(const Rational<U> &lhs, const Rational<V> &rhs) {
  using R = std::common_type_t<U, V>;
  using SR = std::make_signed_t<R>;
  auto a = SR(lhs.sign_) * SR(lhs.nominator_) * SR(rhs.denominator_);
  auto b = SR(rhs.sign_) * SR(rhs.nominator_) * SR(lhs.denominator_);
  auto c = lhs.denominator_ * rhs.denominator_;
  a -= b;
  int s = 1;
  if (a < 0) {
    s = -1;
    a = -a;
  }
  return Rational<R>(a, c, s);
}

template <typename U, typename V>
inline auto operator*(const Rational<U> &lhs, const Rational<V> &rhs) {
  using R = std::common_type_t<U, V>;
  auto s = lhs.sign_ * rhs.sign_;
  auto x = R(lhs.nominator_) * R(rhs.nominator_);
  auto y = R(lhs.denominator_) * R(rhs.denominator_);
  return Rational<R>(x, y, s);
}

template <typename U, typename V>
inline auto operator/(const Rational<U> &lhs, const Rational<V> &rhs) {
  using R = std::common_type_t<U, V>;
  auto s = lhs.sign_ * rhs.sign_;
  auto x = R(lhs.nominator_) * R(rhs.denominator_);
  auto y = R(lhs.denominator_) * R(rhs.nominator_);
  return Rational<R>(x, y, s);
}

} // namespace Spp::SmartNumDetail

#endif // !RATIONAL_H
