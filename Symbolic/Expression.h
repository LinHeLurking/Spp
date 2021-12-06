#ifndef SPP_EXPRESSION_H
#define SPP_EXPRESSION_H

#include "cstdint"
#include <functional>
#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <variant>

#include "Number.h"

namespace Spp {

    using std::dynamic_pointer_cast;
    using std::static_pointer_cast;
    using std::make_shared;

    namespace ExprImpl {
        // L1: +, -
        // L2: *, /
        // L3: ^
        // LN_OP: Numeric
        constexpr int L1_OP = 1, L2_OP = 2, L3_OP = 3, LN_OP = 10000;

        enum ExprNodeType {
            Num, Add, Sub, Mul, Div, Var
        };

        class NodeBase {
        public:
            explicit NodeBase(ExprNodeType type, int priority) : type_(type), priority_(priority) {}

            const ExprNodeType type_;
            const int priority_;
        };

        using SharedNode = std::shared_ptr<NodeBase>;

        class NumericNode : public NodeBase {
        public:
            template<typename ValueT>
            explicit NumericNode(ValueT val) : val_(val), NodeBase(ExprNodeType::Num, LN_OP) {}

            const Numeric::Number val_;
        };

        class ArithmeticNode : public NodeBase {
        public:
            explicit ArithmeticNode(ExprNodeType type, int priority, std::string name,
                                    SharedNode left, SharedNode right)
                    : name_(name), NodeBase(type, priority),
                      left_(std::move(left)), right_(std::move(right)) {}

            const std::string name_;
            SharedNode left_, right_;
        };

        class AddNode : public ArithmeticNode {
        public:
            explicit AddNode(SharedNode left, SharedNode right)
                    : ArithmeticNode(ExprNodeType::Add, L1_OP, "+", std::move(left), std::move(right)) {}
        };

        class SubNode : public ArithmeticNode {
        public:
            explicit SubNode(SharedNode left, SharedNode right)
                    : ArithmeticNode(ExprNodeType::Sub, L1_OP, "-", std::move(left), std::move(right)) {}
        };

        class MulNode : public ArithmeticNode {
        public:
            explicit MulNode(SharedNode left, SharedNode right)
                    : ArithmeticNode(ExprNodeType::Mul, L2_OP, "*", std::move(left), std::move(right)) {}
        };

        class DivNode : public ArithmeticNode {
        public:
            explicit DivNode(SharedNode left, SharedNode right)
                    : ArithmeticNode(ExprNodeType::Div, L2_OP, "/", std::move(left), std::move(right)) {}
        };

        SharedNode add(const SharedNode& a, const SharedNode& b) {
            return static_pointer_cast<NodeBase>(make_shared<AddNode>(a, b));
        }

        SharedNode sub(const SharedNode& a, const SharedNode& b) {
            return static_pointer_cast<NodeBase>(make_shared<SubNode>(a, b));
        }

        SharedNode mul(const SharedNode& a, const SharedNode& b) {
            return static_pointer_cast<NodeBase>(make_shared<MulNode>(a, b));
        }

        SharedNode div(const SharedNode& a, const SharedNode& b) {
            return static_pointer_cast<NodeBase>(make_shared<DivNode>(a, b));
        }


        inline std::string wrap_bracket(const std::string &s) {
            return "(" + s + ")";
        }

        inline std::string to_string(const SharedNode& nd) {
            using std::string;
            using Numeric::to_string;
            switch (nd->type_) {
                case ExprImpl::Num: {
                    auto cur = static_pointer_cast<NumericNode>(nd);
                    return to_string(cur->val_);
                }
                case ExprImpl::Add:
                case ExprImpl::Sub:
                case ExprImpl::Mul:
                case ExprImpl::Div: {
                    auto cur = static_pointer_cast<ExprImpl::ArithmeticNode>(nd);
                    string l_str = to_string(cur->left_);
                    if (cur->left_->priority_ < cur->priority_) {
                        l_str = wrap_bracket(l_str);
                    }
                    string r_str = to_string(cur->right_);
                    if (cur->right_->priority_ < cur->priority_) {
                        r_str = wrap_bracket(r_str);
                    }
                    return l_str + " " + cur->name_ + " " + r_str;
                }
                default:
                    return "unknown";
            }
        }
        
        inline SharedNode eval(SharedNode nd) {
            if (nd->type_ == Add || nd->type_ == Sub || nd->type_ == Mul || nd->type_ == Div) {
                auto cur = static_pointer_cast<ArithmeticNode>(nd);
                auto l_ptr = eval(cur->left_);
                auto r_ptr = eval(cur->right_);
                if (l_ptr->type_ == Num && r_ptr->type_ == Num) {
                    auto l = static_pointer_cast<NumericNode>(l_ptr);
                    auto r = static_pointer_cast<NumericNode>(r_ptr);
                    switch (cur->type_) {
                        case Add:
                            return make_shared<NumericNode>(l->val_ + r->val_);
                        case Sub:
                            return make_shared<NumericNode>(l->val_ - r->val_);
                        case Mul:
                            return make_shared<NumericNode>(l->val_ * r->val_);
                        case Div:
                            return make_shared<NumericNode>(l->val_ / r->val_);
                        default: // impossible
                            return cur;
                    }
                } else {
                    cur->left_ = l_ptr;
                    cur->right_ = r_ptr;
                    return cur;
                }
            } else {
                return nd;
            }
        }
    }

    using std::make_shared;
    using ExprImpl::NumericNode;
    using ExprImpl::SharedNode;

    
    class Expr {
    public:
        template<typename ValueT>
        Expr(ValueT val) : nd_(make_shared<NumericNode>(val)) {}

        inline const ExprImpl::SharedNode get_expr_tree() const {
            return nd_;
        }

        Expr operator+(const Expr &other) const {
            return Expr(ExprImpl::add(nd_, other.nd_));
        }

        Expr operator-(const Expr &other) const {
            return Expr(ExprImpl::sub(nd_, other.nd_));
        }

        Expr operator*(const Expr &other) const {
            return Expr(ExprImpl::mul(nd_, other.nd_));
        }

        Expr operator/(const Expr &other) const {
            return Expr(ExprImpl::div(nd_, other.nd_));
        }

        inline std::string to_string() const {
            return ExprImpl::to_string(nd_);
        }

        inline Expr eval() const {
            return Expr(ExprImpl::eval(nd_));
        }

        friend std::ostream &operator<<(std::ostream &os, const Expr &term) {
            os << term.to_string();
            return os;
        }

    protected:
        ExprImpl::SharedNode nd_;

        explicit Expr(SharedNode nd) : nd_(std::move(nd)) {}
    };
}

#endif //SPP_EXPRESSION_H
