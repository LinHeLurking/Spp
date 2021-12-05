#ifndef SPP_EXPRESSION_H
#define SPP_EXPRESSION_H

#include "cstdint"
#include <functional>
#include <utility>
#include <vector>
#include <string>
#include <memory>

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
            Num, Add, Sub, Mul, Div
        };

        template<typename ValueT>
        class NodeBase {
        public:
            explicit NodeBase<ValueT>(ExprNodeType type, int priority) : type_(type), priority_(priority) {}

            const ExprNodeType type_;
            const int priority_;
        };

        template<typename ValueT>
        using SharedNode = std::shared_ptr<NodeBase<ValueT>>;

        template<typename ValueT>
        class NumericNode : public NodeBase<ValueT> {
        public:
            explicit NumericNode(ValueT val) : val_(val), NodeBase<ValueT>(ExprNodeType::Num, LN_OP) {}

            const ValueT val_;
        };

        template<typename ValueT>
        class ArithmeticNode : public NodeBase<ValueT> {
        public:

            explicit ArithmeticNode(ExprNodeType type, int priority, std::string name,
                                    SharedNode<ValueT> left, SharedNode<ValueT> right)
                    : name_(name), NodeBase<ValueT>(type, priority),
                      left_(std::move(left)), right_(std::move(right)) {}

            const std::string name_;
            SharedNode<ValueT> left_, right_;
        };

        template<typename ValueT>
        class AddNode : public ArithmeticNode<ValueT> {
        public:
            explicit AddNode(SharedNode<ValueT> left, SharedNode<ValueT> right)
                    : ArithmeticNode<ValueT>(ExprNodeType::Add, L1_OP, "+", std::move(left), std::move(right)) {}
        };

        template<typename ValueT>
        class SubNode : public ArithmeticNode<ValueT> {
        public:
            explicit SubNode(SharedNode<ValueT> left, SharedNode<ValueT> right)
                    : ArithmeticNode<ValueT>(ExprNodeType::Sub, L1_OP, "-", std::move(left), std::move(right)) {}
        };

        template<typename ValueT>
        class MulNode : public ArithmeticNode<ValueT> {
        public:
            explicit MulNode(SharedNode<ValueT> left, SharedNode<ValueT> right)
                    : ArithmeticNode<ValueT>(ExprNodeType::Mul, L2_OP, "*", std::move(left), std::move(right)) {}
        };

        template<typename ValueT>
        class DivNode : public ArithmeticNode<ValueT> {
        public:
            explicit DivNode(SharedNode<ValueT> left, SharedNode<ValueT> right)
                    : ArithmeticNode<ValueT>(ExprNodeType::Div, L2_OP, "/", std::move(left), std::move(right)) {}
        };

        template<typename ValueT>
        SharedNode<ValueT> add(SharedNode<ValueT> a, SharedNode<ValueT> b) {
            return static_pointer_cast<NodeBase<ValueT>>(make_shared<AddNode<ValueT>>(a, b));
        }

        template<typename ValueT>
        SharedNode<ValueT> sub(SharedNode<ValueT> a, SharedNode<ValueT> b) {
            return static_pointer_cast<NodeBase<ValueT>>(make_shared<SubNode<ValueT>>(a, b));
        }

        template<typename ValueT>
        SharedNode<ValueT> mul(SharedNode<ValueT> a, SharedNode<ValueT> b) {
            return static_pointer_cast<NodeBase<ValueT>>(make_shared<MulNode<ValueT>>(a, b));
        }

        template<typename ValueT>
        SharedNode<ValueT> div(SharedNode<ValueT> a, SharedNode<ValueT> b) {
            return static_pointer_cast<NodeBase<ValueT>>(make_shared<DivNode<ValueT>>(a, b));
        }

        inline std::string wrap_bracket(const std::string &s) {
            return "(" + s + ")";
        }

        template<typename ValueT>
        inline std::string to_string(const SharedNode<ValueT> nd) {
            using std::string;
            using std::to_string;
            using Numeric::to_string;
            switch (nd->type_) {
                case ExprImpl::Num: {
                    auto cur = static_pointer_cast<NumericNode<ValueT>>(nd);
                    return to_string(cur->val_);
                }
                case ExprImpl::Add:
                case ExprImpl::Sub:
                case ExprImpl::Mul:
                case ExprImpl::Div: {
                    auto cur = static_pointer_cast<ExprImpl::AddNode<ValueT>>(nd);
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

        template<typename ValueT>
        inline SharedNode<ValueT> eval(SharedNode<ValueT> nd) {
            if (nd->type_ == Add || nd->type_ == Sub || nd->type_ == Mul || nd->type_ == Div) {
                auto cur = static_pointer_cast<ArithmeticNode<ValueT>>(nd);
                auto l_ptr = eval(cur->left_);
                auto r_ptr = eval(cur->right_);
                if (l_ptr->type_ == Num && r_ptr->type_ == Num) {
                    auto l = static_pointer_cast<NumericNode<ValueT>>(l_ptr);
                    auto r = static_pointer_cast<NumericNode<ValueT>>(r_ptr);
                    switch (cur->type_) {
                        case Add:
                            return make_shared<NumericNode<ValueT>>(l->val_ + r->val_);
                        case Sub:
                            return make_shared<NumericNode<ValueT>>(l->val_ - r->val_);
                        case Mul:
                            return make_shared<NumericNode<ValueT>>(l->val_ * r->val_);
                        case Div:
                            return make_shared<NumericNode<ValueT>>(l->val_ / r->val_);
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

        template<typename FromT, typename ToT>
        inline SharedNode<ToT> cast(const SharedNode<FromT> nd) {
            if (nd->type_ == Add || nd->type_ == Sub || nd->type_ == Mul || nd->type_ == Div) {
                auto cur = static_pointer_cast<ArithmeticNode<FromT>>(nd);
                auto l = cast<FromT, ToT>(cur->left_);
                auto r = cast<FromT, ToT>(cur->right_);
                return make_shared<ArithmeticNode<ToT>>(cur->type_, cur->priority_, cur->name_, l, r);
            } else { // nd->type_ == Num
                auto cur = static_pointer_cast<NumericNode<FromT>>(nd);
                return make_shared<NumericNode<ToT>>(ToT(cur->val_));
            }
        }
    }

    using std::make_shared;
    using ExprImpl::NumericNode;
    using ExprImpl::SharedNode;


    template<typename ValueT>
    class Expr {
    public:
        explicit Expr(ValueT val) : nd_(make_shared<NumericNode<ValueT>>(val)) {}

        inline const ExprImpl::SharedNode<ValueT> get_expr_tree() const {
            return nd_;
        }

        template<class AnotherValueT>
        explicit Expr(const Expr<AnotherValueT> &expr) {
            this->nd_ = ExprImpl::cast<AnotherValueT, ValueT>(expr.get_expr_tree());
        }

        Expr<ValueT> operator+(const Expr<ValueT> &other) {
            return Expr<ValueT>(ExprImpl::add(nd_, other.nd_));
        }

        Expr<ValueT> operator-(const Expr<ValueT> &other) {
            return Expr<ValueT>(ExprImpl::sub(nd_, other.nd_));
        }

        Expr<ValueT> operator*(const Expr<ValueT> &other) {
            return Expr<ValueT>(ExprImpl::mul(nd_, other.nd_));
        }

        Expr<ValueT> operator/(const Expr<ValueT> &other) {
            return Expr<ValueT>(ExprImpl::div(nd_, other.nd_));
        }

        inline std::string to_string() const {
            return ExprImpl::to_string(nd_);
        }

        inline Expr<ValueT> eval() {
            return Expr<ValueT>(ExprImpl::eval(nd_));
        }

        template<typename ToType>
        inline Expr<ToType> as_type() const {
            return Expr<ToType>(*this);
        }

        friend std::ostream &operator<<(std::ostream &os, const Expr<ValueT> &term) {
            os << term.to_string();
            return os;
        }

    protected:
        ExprImpl::SharedNode<ValueT> nd_;

        explicit Expr(SharedNode<ValueT> nd) : nd_(std::move(nd)) {}
    };

    template<typename ValueT>
    inline std::string to_string(const Expr<ValueT> &term) {
        return term.to_string();
    }
}

#endif //SPP_EXPRESSION_H
