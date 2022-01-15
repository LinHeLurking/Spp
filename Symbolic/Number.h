#ifndef SPP_NUMBER_H
#define SPP_NUMBER_H

#include <type_traits>
#include <algorithm>
#include <numeric>
#include <string>
#include <iostream>
#include <cstdint>
#include <variant>
#include <cmath>

namespace Spp::Numeric {
    using Integer = int64_t;
    using Real = double;

    struct Rational {
        int64_t n_, d_; // nominator & denominator

        explicit Rational(int64_t n, int64_t d) : n_(n), d_(d) {
            int64_t k = std::gcd(n, d);
            n_ /= k;
            d_ /= k;
        }

        explicit Rational(int64_t n) : n_(n), d_(1) {}

        inline void reduce() {
            int64_t k = std::gcd(n_, d_);
            n_ /= k;
            d_ /= k;
        }

        template<typename U, std::enable_if_t<(std::is_integral_v<U> || std::is_floating_point_v<U>), bool> = true>
        explicit operator U() const {
            return static_cast<U>(n_) / static_cast<U>(d_);
        }

        [[nodiscard]] Rational inverse() const {
            return Rational(d_, n_);
        }

        Rational operator+=(Rational other) {
            n_ = n_ * other.d_ + other.n_ * d_;
            d_ *= other.d_;
            reduce();
            return *this;
        }

        Rational operator-=(Rational other) {
            n_ = n_ * other.d_ - other.n_ * d_;
            d_ *= other.d_;
            reduce();
            return *this;
        }

        Rational operator*=(Rational other) {
            n_ *= other.n_;
            d_ *= other.d_;
            reduce();
            return *this;
        }

        Rational operator/=(Rational other) {
            n_ *= other.d_;
            d_ *= other.n_;
            reduce();
            return *this;
        }
    };

    template<typename>
    struct is_rational : std::false_type {
    };

    template<>
    struct is_rational<Rational> : std::true_type {
    };

    template<typename T>
    inline constexpr bool is_rational_v = is_rational<T>::value;

    template<typename T>
    inline constexpr bool is_numeric_v = std::is_integral_v<T> || std::is_floating_point_v<T> || is_rational_v<T>;

    auto operator+(Rational self, Rational other) {
        Rational res = self;
        res += other;
        return res;
    }

    auto operator-(Rational self, Rational other) {
        Rational res = self;
        res -= other;
        return res;
    }

    auto operator*(Rational self, Rational other) {
        Rational res = self;
        res *= other;
        return res;
    }

    auto operator/(Rational self, Rational other) {
        Rational res = self;
        res /= other;
        return res;
    }

    class Number {
    public:
        explicit Number(int64_t x) : val_(x) {}

        explicit Number(double x) : val_(x) {}

        explicit Number(Rational x) : val_(x) {}

        template<typename ValueT>
        inline ValueT get() const {
            return std::get<ValueT>(val_);
        }

        template<typename ValueT>
        inline ValueT as() const {
            switch (val_.index()) {
                case 0:
                    return ValueT(std::get<Integer>(val_));
                case 1:
                    return ValueT(std::get<Rational>(val_));
                case 2:
                    return ValueT(std::get<Real>(val_));
                default: // Impossible
                    throw std::runtime_error("Error getting!");
            }
        }

        inline int index() const {
            return val_.index();
        }

    protected:
        std::variant<Integer, Rational, Real> val_;
    };

    inline Number operator+(Number a, Number b) {
        int common_index = std::max(a.index(), b.index());
        switch (common_index) {
            case 0:
                return Number{a.get<Integer>() + b.get<Integer>()};
            case 1:
                return Number{a.as<Rational>() + b.as<Rational>()};
            case 2:
                return Number{a.as<Real>() + b.as<Real>()};
            default: // Impossible
                throw std::runtime_error("Illegal arithmetic op!");
        }
    }

    inline Number operator-(Number a, Number b) {
        int common_index = std::max(a.index(), b.index());
        switch (common_index) {
            case 0:
                return Number{a.get<Integer>() - b.get<Integer>()};
            case 1:
                return Number{a.as<Rational>() - b.as<Rational>()};
            case 2:
                return Number{a.as<Real>() - b.as<Real>()};
            default: // Impossible
                throw std::runtime_error("Illegal arithmetic op!");
        }
    }

    inline Number operator*(Number a, Number b) {
        int common_index = std::max(a.index(), b.index());
        switch (common_index) {
            case 0:
                return Number{a.get<Integer>() * b.get<Integer>()};
            case 1:
                return Number{a.as<Rational>() * b.as<Rational>()};
            case 2:
                return Number{a.as<Real>() * b.as<Real>()};
            default: // Impossible
                throw std::runtime_error("Illegal arithmetic op!");
        }
    }

    inline Number operator/(Number a, Number b) {
        int common_index = std::max(a.index(), b.index());
        switch (common_index) {
            case 0:
                return Number{Rational(a.get<Integer>()) / Rational(b.get<Integer>())};
            case 1:
                return Number{a.as<Rational>() / b.as<Rational>()};
            case 2:
                return Number{a.as<Real>() / b.as<Real>()};
            default: // Impossible
                throw std::runtime_error("Illegal arithmetic op!");
        }
    }

    std::string to_string(const Rational &v) {
        using std::to_string;
        if (v.d_ == 1) {
            return std::to_string(v.n_);
        } else {
            return std::to_string(v.n_) + "/" + std::to_string(v.d_);
        }
    }

    std::string to_string(const Number &v) {
        using std::to_string;
        switch (v.index()) {
            case 0:
                return to_string(v.get<Integer>());
            case 1:
                return to_string(v.get<Rational>());
            case 2:
                return to_string(v.get<Real>());
            default:
                return "unknown value";
        }
    }

    inline Number pow(const Number &a, int64_t b) {
        Number ans = Number{1L};
        Number tmp = a;
        if (b < 0) {
            tmp = Number{1L} / tmp;
            b = -b;
        }
        while (b) {
            if (b & 1) {
                ans = ans * tmp;
            }
            tmp = tmp * tmp;
            b >>= 1;
        }
        return ans;
    }

    inline Number pow(const Number &a, const Number &b) {
        if (b.index() == 0) {
            return pow(a, b.get<int64_t>());
        } else {
            Real base = a.as<Real>(), p = b.as<Real>();
            return Number{std::pow(base, p)};
        }
    }
}

#endif //SPP_NUMBER_H
