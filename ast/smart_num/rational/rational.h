#ifndef SPP_SMART_NUM_RATIONAL_H
#define SPP_SMART_NUM_RATIONAL_H

#include <cstdint>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <type_traits>

#include "../../../util/concept.h"

namespace Spp::__SmartNum::__Detail {

// using namespace Spp::Concept;
using Spp::Concept::SignedInteger;
using Spp::Concept::UnsignedInteger;

template <typename T, typename U>
using CT = std::common_type_t<T, U>;

template <typename T = uint64_t>
requires UnsignedInteger<T>
class Rational {
 public:
  Rational(T nominator, T denominator, int sign = 1)
      : nominator_(nominator), denominator_(denominator) {
    if (denominator_ == 0) {
      throw std::runtime_error("Zero denominator!");
    }
    sign_ = sign >= 0 ? 1 : -1;
    this->reduce();
  }

  template <typename U>
  requires std::is_integral_v<U> Rational(U k) {
    sign_ = k >= 0 ? 1 : -1;
    nominator_ = k >= 0 ? k : -k;
    denominator_ = 1;
  }

  template <typename U>
  requires std::is_floating_point_v<U> Rational(U x) {
    throw std::runtime_error(
        "Do not call float/double to Rational "
        "constructor! This is an intended error.");
  }

  // User-defined cast
  template <typename U>
  requires std::is_floating_point_v<U> || SignedInteger<U>
  explicit operator U() const {
    return U(sign_) * U(nominator_) / U(denominator_);
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
  requires std::is_floating_point_v<U>
  inline bool operator==(const U rhs) const { return false; }

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
  requires std::is_floating_point_v<U>
  inline auto operator+(const U rhs) const { return U(*this) + rhs; }

  template <typename U>
  requires std::is_floating_point_v<U>
  friend inline auto operator+(const U lhs, const Rational &rhs) {
    return lhs + U(rhs);
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
  requires std::is_floating_point_v<U>
  inline auto operator-(const U rhs) const { return U(*this) - rhs; }

  template <typename U>
  requires std::is_floating_point_v<U>
  friend inline auto operator-(const U lhs, const Rational &rhs) {
    return lhs - U(rhs);
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
  requires std::is_floating_point_v<U>
  inline auto operator*(const U rhs) const { return U(*this) * rhs; }

  template <typename U>
  requires std::is_floating_point_v<U>
  friend inline auto operator*(const U lhs, const Rational &rhs) {
    return lhs * U(rhs);
  }

  template <typename U>
  requires std::is_integral_v<U>
  inline auto operator/(const U rhs) const {
    auto r = make_rational(rhs);
    return *this / r;
  }

  template <typename U>
  requires std::is_integral_v<U>
  friend inline auto operator/(const U lhs, const Rational &rhs) {
    auto l = make_rational(lhs);
    return l / rhs;
  }

  template <typename U>
  requires std::is_floating_point_v<U>
  inline auto operator/(const U rhs) const { return U(*this) / rhs; }

  template <typename U>
  requires std::is_floating_point_v<U>
  friend inline auto operator/(const U lhs, const Rational &rhs) {
    return lhs / U(rhs);
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

  /**
   * I/O overload.
   */
  friend inline std::ostream &operator<<(std::ostream &os, const Rational &r) {
    if (r.sign_ == -1) {
      os << "-";
    }
    os << r.nominator_;
    if (r.denominator_ > 1) {
      os << '/' << r.denominator_;
    }
    os << std::endl;
    return os;
  }

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

/**
 * Definitions
 */

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
}  // namespace Spp::__SmartNum::__Detail

#endif  // !SPP_SMART_NUM_RATIONAL_H
