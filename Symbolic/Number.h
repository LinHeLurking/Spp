#ifndef SPP_NUMBER_H
#define SPP_NUMBER_H

#include <type_traits>
#include <algorithm>
#include <numeric>
#include <string>
#include <iostream>

namespace Spp::Numeric {
    template<typename T>
    struct Rational {
        T n_, d_; // nominator & denominator

        template<std::enable_if_t<std::is_integral_v<T>, bool> = true>
        explicit Rational(T n, T d) : n_(n), d_(d) {
            T k = std::gcd(n, d);
            n_ /= k;
            d_ /= k;
        }

        template<std::enable_if_t<std::is_integral_v<T>, bool> = true>
        explicit Rational(T n) : n_(n), d_(1) {
        }

        inline void reduce() {
            T k = std::gcd(n_, d_);
            n_ /= k;
            d_ /= k;
        }

        template<typename U, std::enable_if_t<std::is_arithmetic_v<U>, bool> = true>
        explicit operator U() {
            return static_cast<U>(n_) / static_cast<U>(d_);
        }

        Rational<T> inverse() const {
            return Rational(d_, n_);
        }

        template<typename FloatT, std::enable_if_t<std::is_floating_point_v<FloatT>, bool> = true>
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
        Rational<T> operator-=(Rational<U> other) {
            n_ = n_ * other.d_ - other.n_ * d_;
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
    };

    using std::to_string;

    template<typename ValueT>
    std::string to_string(const Rational<ValueT> &v) {
        return std::to_string(v.n_) + "/" + std::to_string(v.d_);
    }

    template<typename>
    struct is_rational : std::false_type {
    };

    template<typename T>
    struct is_rational<Rational<T>> : std::true_type {
    };

    template<typename T>
    inline constexpr bool is_rational_v = is_rational<T>::value;

    template<typename T, typename U>
    auto operator+(Rational<T> self, Rational<U> other) {
        Rational<std::common_type_t<T, U>> res = self;
        res += other;
        return res;
    }

    template<typename T, typename U>
    auto operator-(Rational<T> self, Rational<U> other) {
        Rational<std::common_type_t<T, U>> res = self;
        res -= other;
        return res;
    }

    template<typename T, typename U>
    auto operator*(Rational<T> self, Rational<U> other) {
        Rational<std::common_type_t<T, U>> res = self;
        res *= other;
        return res;
    }

    template<typename T, typename U>
    auto operator/(Rational<T> self, Rational<U> other) {
        Rational<std::common_type_t<T, U>> res = self;
        res /= other;
        return res;
    }

    template<typename T, typename U, std::enable_if_t<
            std::is_integral_v<U> || std::is_floating_point_v<U>, bool> = true>
    auto operator+(const Rational<T> a, const U b) {
        if constexpr(std::is_integral_v<U>) {
            return a + static_cast<Rational<U>>(b);
        } else if constexpr(std::is_floating_point_v<U>) {
            return static_cast<U>(a) + b;
        }
    }

    template<typename T, typename U, std::enable_if_t<!is_rational_v<T>, bool> = true>
    auto operator+(const T a, const Rational<U> b) {
        return b + a;
    }

    template<typename T, typename U, std::enable_if_t<
            std::is_integral_v<U> || std::is_floating_point_v<U>, bool> = true>
    auto operator-(const Rational<T> a, const U b) {
        if constexpr(std::is_integral_v<U>) {
            return a - static_cast<Rational<U>>(b);
        } else if constexpr(std::is_floating_point_v<U>) {
            return static_cast<U>(a) - b;
        }
    }

    template<typename T, typename U, std::enable_if_t<!is_rational_v<T>, bool> = true>
    auto operator-(const T a, const Rational<U> b) {
        if constexpr(std::is_integral_v<T>) {
            return static_cast<Rational<T>>(a) - b;
        } else if constexpr(std::is_floating_point_v<T>) {
            return a - static_cast<T>(b);
        }
    }

    template<typename T, typename U, std::enable_if_t<std::is_arithmetic_v<U>, bool> = true>
    auto operator*(const Rational<T> a, const U b) {
        if constexpr(std::is_integral_v<U>) {
            return a * static_cast<Rational<U>>(b);
        } else if constexpr(std::is_floating_point_v<U>) {
            return static_cast<U>(a) * b;
        }
    }

    template<typename T, typename U, std::enable_if_t<!is_rational_v<T>, bool> = true>
    auto operator*(const T a, const Rational<U> b) {
        return b * a;
    }

    template<typename T>
    inline constexpr bool is_numeric_v = std::is_integral_v<T> || std::is_floating_point_v<T> || is_rational_v<T>;
}

#endif //SPP_NUMBER_H
