#ifndef SPP_COMPUTE_H
#define SPP_COMPUTE_H

#include "cstdint"
#include <utility>
#include <vector>
#include <string>
#include <sstream>
#include <variant>
#include <map>

#include "Number.h"

namespace Spp::Compute {
    using std::string;
    using Numeric::Number;
    using std::vector;
    enum OpType {
        Add, Sub, Mul, Div, Pow, Function
    };

    struct Variable {
        string name_;

        explicit Variable(const string &s) {
            bool flag = true;
            for (int i = 0; i < s.size() && flag; ++i) {
                flag &= (s[i] == '_' || s[i] >= 'a' || s[i] <= 'z' || s[i] >= 'A' || s[i] <= 'Z');
            }
            if (flag) {
                name_ = s;
            } else {
                throw std::runtime_error("Bad variable name: only accept a-zA-Z and underline(_) in variable names!");
            }
        }
    };

    string to_string(const Variable &variable) {
        return variable.name_;
    }

    constexpr int L0_OP = 0, L1_OP = 1, L2_OP = 2, L3_OP = 3, LN_OP = 10000;

    struct Op {
        OpType type_;
        const int priority_;
        const string name_;

        Op(OpType t, int p, string n) : type_(t), priority_(p), name_(std::move(n)) {}
    };

    struct AddOp : Op {
        AddOp() : Op(Add, L1_OP, "+") {}
    };

    struct SubOp : Op {
        SubOp() : Op(Sub, L1_OP, "-") {}
    };

    struct MulOp : Op {
        MulOp() : Op(Mul, L2_OP, "*") {}
    };

    struct DivOp : Op {
        DivOp() : Op(Div, L2_OP, "/") {}
    };

    struct PowOp : Op {
        PowOp() : Op(Pow, L3_OP, "^") {}
    };

//    using Term = std::variant<Number, Variable>;
    using Node = std::variant<Number, Variable, Op>;
    constexpr int INDEX_NUMBER = 0, INDEX_VARIABLE = 1, INDEX_OP = 2;

    struct Expr {
        Node root_;
        vector<Expr> children_;

        explicit Expr(Number x) : root_(x) {}

        explicit Expr(Variable x) : root_(x) {}

        explicit Expr(const string &s) : root_(Variable(s)) {}

        explicit Expr(int64_t x) : root_(Number{x}) {}

        explicit Expr(int x) : root_(Number{int64_t(x)}) {}

        explicit Expr(double x) : root_(Number{x}) {}

        explicit Expr(Numeric::Rational x) : root_(Number{x}) {}

        Expr(const Op &op, const vector<Expr> &operand) : root_(op), children_(operand) {}

        Expr operator+(const Expr &rhs) const {
            return Expr{AddOp(), {*this, rhs}};
        }

        Expr operator-(const Expr &rhs) const {
            return Expr{SubOp(), {*this, rhs}};
        }

        Expr operator*(const Expr &rhs) const {
            return Expr{MulOp(), {*this, rhs}};
        }

        Expr operator/(const Expr &rhs) const {
            return Expr{DivOp(), {*this, rhs}};
        }

        // raise this to x power: return (*this)^x.
        Expr pow(const Expr &x) const;

        [[nodiscard]] inline bool is_single_number() const {
            return root_.index() == INDEX_NUMBER && children_.empty();
        }

        [[nodiscard]] inline bool is_single_variable() const {
            return root_.index() == INDEX_VARIABLE && children_.empty();
        }

        [[nodiscard]] inline int root_priority() const {
            if (root_.index() != INDEX_OP) {
                return LN_OP;
            } else {
                return std::get<Op>(root_).priority_;
            }
        }

        [[nodiscard]] inline Number as_number() const {
            if (this->is_single_number()) {
                return std::get<Number>(root_);
            } else {
                throw std::runtime_error("Bad conversion: this is expression is not a number!");
            }
        }

        [[nodiscard]] inline Variable as_variable() const {
            if (this->is_single_variable()) {
                return std::get<Variable>(root_);
            } else {
                throw std::runtime_error("Bad conversion: this is expression is not a variable!");
            }
        }

        [[nodiscard]] string to_string() const;

        Expr eval() const;
    };

    string to_string(const Expr &expr) {
        std::stringstream res;
        if (expr.root_.index() == INDEX_OP) {
            const auto &op = std::get<Op>(expr.root_);
            switch (op.type_) {
                case Add:
                case Sub:
                case Mul:
                case Div:
                case Pow: {
                    bool left_child_bracket = expr.children_[0].root_priority() < expr.root_priority();
                    bool right_child_bracket = expr.children_[1].root_priority() < expr.root_priority();
                    res << (left_child_bracket ? "(" : "")
                        << to_string(expr.children_[0])
                        << (left_child_bracket ? ")" : "");
                    res << op.name_;
                    res << (right_child_bracket ? "(" : "")
                        << to_string(expr.children_[1])
                        << (right_child_bracket ? ")" : "");
                }
                case Function:
                    break;
            }
        } else if (expr.root_.index() == INDEX_VARIABLE) {
            res << to_string(expr.as_variable());
        } else if (expr.root_.index() == INDEX_NUMBER) {
            res << to_string(expr.as_number());
        }
        return res.str();
    }

    // Raise power, return a^b
    Expr pow(const Expr &a, const Expr &b) {
        return Expr(PowOp(), {a, b});
    }

    Expr Expr::pow(const Expr &x) const {
        return Spp::Compute::pow(*this, x);
    }

    string Expr::to_string() const {
        return Spp::Compute::to_string(*this);
    }

    Expr eval(const Expr &expr) {
        using Numeric::pow;
        if (expr.root_.index() != INDEX_OP) {
            return expr;
        } else {
            const auto &op = std::get<Op>(expr.root_);
            switch (op.type_) {
                case Function:
                    return expr;
                default: {
                    const auto left = eval(expr.children_[0]);
                    const auto right = eval(expr.children_[1]);
                    bool combine = left.is_single_number() && right.is_single_number();
                    switch (op.type_) {
                        case Add:
                            return combine ? Expr(left.as_number() + right.as_number()) : left + right;
                        case Sub:
                            return combine ? Expr(left.as_number() - right.as_number()) : left - right;
                        case Mul:
                            return combine ? Expr(left.as_number() * right.as_number()) : left * right;
                        case Div:
                            return combine ? Expr(left.as_number() / right.as_number()) : left / right;
                        case Pow:
                            return combine ? Expr(pow(left.as_number(), right.as_number())) : pow(left, right);
                        default: // Impossible
                            throw std::runtime_error("Impossible evaluation dispatch.");
                    }
                }
            }
        }
    }

    Expr Expr::eval() const {
        return Spp::Compute::eval(*this);
    }
}

#endif //SPP_COMPUTE_H
