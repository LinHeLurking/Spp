#ifndef SPP_NUMBER_H
#define SPP_NUMBER_H

#include <type_traits>
#include <algorithm>
#include <numeric>
#include <iostream>

using namespace std;

namespace Spp {
    namespace Numeric {
        template<typename T, enable_if_t<is_integral_v<T>, bool> = true>
        struct Rational {
            T n_, d_; // nominator & denominator

            explicit Rational(T n, T d) : n_(n), d_(d) {
                T k = gcd(n, d);
                n_ /= k;
                d_ /= k;
            }

            explicit Rational(T n) : n_(n), d_(1) {
            }

            inline void reduce() {
                T k = gcd(n_, d_);
                n_ /= k;
                d_ /= k;
            }

            Rational<T> inverse() const {
                return Rational(d_, n_);
            }

            template<typename FloatT, enable_if_t<is_floating_point_v<FloatT>, bool> = true>
            explicit operator FloatT() const {
                return static_cast<FloatT>(n_) / static_cast<FloatT>(d_);
            }

            template<typename U>
            Rational<T> operator+=(Rational<U> other) {
                n_ = n_ * other.d_ + other.n_ * d_;
                d_ *= other.d_;
                reduce();
                return *this;
            }

            template<typename U>
            Rational<T> operator*=(Rational<U> other) {
                n_ *= other.n_;
                d_ *= other.d_;
                reduce();
                return *this;
            }

            Rational<T> operator/=(Rational<T> other) {
                n_ *= other.d_;
                d_ *= other.n_;
                reduce();
                return *this;
            }

            Rational<T> operator/(Rational<T> other) {
                auto a = *this;
                a /= other;
                return a;
            }

            friend ostream &operator<<(ostream &os, Rational<T> r) {
                os << r.n_ << " / " << r.d_;
                return os;
            }
        };

        template<typename>
        struct is_rational : false_type {
        };

        template<typename T>
        struct is_rational<Rational<T>> : true_type {
        };

        template<typename T>
        inline constexpr bool is_rational_v = is_rational<T>::value;

        template<typename T, typename U>
        auto operator+(Rational<T> self, Rational<U> other) {
            Rational<common_type_t<T, U>> res = self;
            res += other;
            return res;
        }

        template<typename T, typename U>
        auto operator*(Rational<T> self, Rational<U> other) {
            Rational<common_type_t<T, U>> res = self;
            res *= other;
            return res;
        }

        template<typename T, typename U>
        auto operator/(Rational<T> self, Rational<U> other) {
            Rational<common_type_t<T, U>> res = self;
            res /= other;
            return res;
        }

        template<typename T, typename U, enable_if_t<is_integral_v<U> || is_floating_point_v<U>, bool> = true>
        auto operator+(const Rational<T> a, const U b) {
            if constexpr(is_integral_v<U>) {
                return a + static_cast<Rational<U>>(b);
            } else if constexpr(is_floating_point_v<U>) {
                return static_cast<U>(a) + b;
            }
        }

        template<typename T, typename U, enable_if_t<!is_rational_v<T>, bool> = true>
        auto operator+(const T a, const Rational<U> b) {
            return b + a;
        }

        template<typename T, typename U, enable_if_t<is_arithmetic_v<U>, bool> = true>
        auto operator*(const Rational<T> a, const U b) {
            if constexpr(is_integral_v<U>) {
                return a * static_cast<Rational<U>>(b);
            } else if constexpr(is_floating_point_v<U>) {
                return static_cast<U>(a) * b;
            }
        }

        template<typename T, typename U, enable_if_t<!is_rational_v<T>, bool> = true>
        auto operator*(const T a, const Rational<U> b) {
            return b * a;
        }

        template<typename T>
        inline constexpr bool is_numeric_v = is_integral_v<T> || is_floating_point_v<T> || is_rational_v<T>;
    }
}

#endif //SPP_NUMBER_H
