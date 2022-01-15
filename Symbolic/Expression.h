#ifndef SPP_EXPRESSION_H
#define SPP_EXPRESSION_H

#include "cstdint"
#include <functional>
#include <utility>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <variant>
#include <map>

#include "Number.h"

namespace Spp {
    using std::string;
    using Numeric::Number;
    using std::vector;
    using std::shared_ptr;
    enum OpType {
        Add, Sub, Mul, Div, Function
    };

    struct Variable {
        string name_;
    };

    constexpr int L0_OP = 0, L1_OP = 1, L2_OP = 2, LN_OP = 10000;

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

//    using Term = std::variant<Number, Variable>;
    using Node = std::variant<Number, Variable, Op>;
    constexpr int INDEX_NUMBER = 0, INDEX_VARIABLE = 1, INDEX_OP = 2;

    struct Expr {
        Node root_;
        vector<Expr> children_;

        explicit Expr(Number x) : root_(x) {}

        explicit Expr(Variable x) : root_(x) {}

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

        [[nodiscard]] inline bool is_single_number() const {
            return root_.index() == 0;
        }

        [[nodiscard]] inline int root_priority() const {
            if (root_.index() != INDEX_OP) {
                return LN_OP;
            } else {
                return std::get<Op>(root_).priority_;
            }
        }
    };

    string to_string(const Expr &expr) {
        std::stringstream res;
        if (expr.root_.index() == INDEX_OP) {
            const auto &op = std::get<Op>(expr.root_);
            switch (op.type_) {
                case Add:
                case Sub:
                case Mul:
                case Div: {
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
            const auto &variable = std::get<Variable>(expr.root_);
            res << variable.name_;
        } else if (expr.root_.index() == INDEX_NUMBER) {
            const auto &number = std::get<Number>(expr.root_);
            res << to_string(number);
        }
        return res.str();
    }
}

#endif //SPP_EXPRESSION_H
