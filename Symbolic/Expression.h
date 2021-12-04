#ifndef SPP_EXPRESSION_H
#define SPP_EXPRESSION_H

#include "cstdint"
#include <functional>
#include <vector>
#include <string>
#include <memory>

#include "Number.h"

namespace Spp::Expr {

    // L1: +, -
    // L2: *, /
    // L3: ^
    // LN_OP: Numeric
    constexpr int L1_OP = 1, L2_OP = 2, L3_OP = 3, LN_OP = 10000;

    enum ExprNodeType {
        Num, Add, Sub, Mul, Div, Cast
    };

    template<typename ValueT>
    class NodeBase {
    public:
        explicit NodeBase(ExprNodeType type, int pred) : pred_(pred), type_(type) {}

        explicit NodeBase() = delete;

        int pred_;
        ExprNodeType type_;

        [[nodiscard]] virtual std::string expr_to_string() const = 0;
    };

    template<typename ValueT>
    using SharedNode = std::shared_ptr<NodeBase<ValueT>>;

    std::string wrap_bracket(const std::string &s) {
        return "(" + s + ")";
    }


    template<typename ValueT>
    class NumericNode : public NodeBase<ValueT> {
    public:
        explicit NumericNode(ValueT val) : val_(val), NodeBase<ValueT>(ExprNodeType::Num, LN_OP) {}

        [[nodiscard]] std::string expr_to_string() const override {
            return to_string(val_);
        }

        ValueT val_;
    };

    template<typename ValueT>
    class OperatorNode : public NodeBase<ValueT> {
    public:
        template<class StringT>
        explicit OperatorNode(ExprNodeType type, int pred, StringT name) :NodeBase<ValueT>(type, pred), name_(name) {}

        virtual ValueT op(const std::vector<ValueT> &v) = 0;

        const std::string name_;

        std::vector<SharedNode<ValueT>> children_;
    };

    template<typename ValueT>
    class ArithmeticNode : public OperatorNode<ValueT> {
    public:
        template<typename T, typename U>
        explicit
        ArithmeticNode(T a, U b, ExprNodeType type, int pred, std::string name)
                : OperatorNode<ValueT>(type, pred, name) {
            this->children_.push_back(SharedNode<ValueT>(new T(a)));
            this->children_.push_back(SharedNode<ValueT>(new U(b)));
        }

        [[nodiscard]] std::string expr_to_string() const override {
            auto left = this->children_[0];
            auto right = this->children_[1];
            auto l_str = left->pred_ < this->pred_ ? wrap_bracket(left->expr_to_string()) : left->expr_to_string();
            auto r_str = right->pred_ < this->pred_ ? wrap_bracket(right->expr_to_string()) : right->expr_to_string();
            return l_str + " " + this->name_ + " " + r_str;
        }
    };

    template<typename ValueT>
    class AddNode : public ArithmeticNode<ValueT> {
    public:
        template<typename T, typename U>
        explicit AddNode(T a, U b)
                :ArithmeticNode<ValueT>(a, b, ExprNodeType::Add, L1_OP, "+") {}

        ValueT op(const std::vector<ValueT> &v) override {
            return v[0] + v[1];
        }
    };

    template<typename ValueT>
    class SubNode : public ArithmeticNode<ValueT> {
    public:
        template<typename T, typename U>
        explicit SubNode(T a, U b)
                :ArithmeticNode<ValueT>(a, b, ExprNodeType::Sub, L1_OP, "-") {}

        ValueT op(const std::vector<ValueT> &v) override {
            return v[0] - v[1];
        }
    };

    template<typename ValueT>
    class MulNode : public ArithmeticNode<ValueT> {
    public:
        template<typename T, typename U>
        explicit MulNode(T a, U b)
                :ArithmeticNode<ValueT>(a, b, ExprNodeType::Mul, L2_OP, "*") {}

        ValueT op(const std::vector<ValueT> &v) override {
            return v[0] * v[1];
        }
    };

    template<typename ValueT>
    class DivNode : public ArithmeticNode<ValueT> {
    public:
        template<typename T, typename U>
        explicit DivNode(T a, U b)
                :ArithmeticNode<ValueT>(a, b, ExprNodeType::Div, L2_OP, "/") {}

        ValueT op(const std::vector<ValueT> &v) override {
            return v[0] / v[1];
        }
    };

    template<typename ValueT,
            template<class> class T,
            template<class> class U,
            std::enable_if_t<
                    (std::is_base_of_v<NodeBase<ValueT>, T<ValueT>> ||
                     std::is_base_of_v<NodeBase<ValueT>, U<ValueT>>), bool> = true>
    AddNode<ValueT> operator+(T<ValueT> a, U<ValueT> b) {
        return AddNode<ValueT>(a, b);
    }

    template<typename ValueT,
            template<class> class T,
            template<class> class U,
            std::enable_if_t<
                    (std::is_base_of_v<NodeBase<ValueT>, T<ValueT>> ||
                     std::is_base_of_v<NodeBase<ValueT>, U<ValueT>>), bool> = true>
    SubNode<ValueT> operator-(T<ValueT> a, U<ValueT> b) {
        return SubNode<ValueT>(a, b);
    }

    template<typename ValueT,
            template<class> class T,
            template<class> class U,
            std::enable_if_t<
                    (std::is_base_of_v<NodeBase<ValueT>, T<ValueT>> ||
                     std::is_base_of_v<NodeBase<ValueT>, U<ValueT>>), bool> = true>
    MulNode<ValueT> operator*(T<ValueT> a, U<ValueT> b) {
        return MulNode<ValueT>(a, b);
    }

    template<typename ValueT,
            template<class> class T,
            template<class> class U,
            std::enable_if_t<
                    (std::is_base_of_v<NodeBase<ValueT>, T<ValueT>> ||
                     std::is_base_of_v<NodeBase<ValueT>, U<ValueT>>), bool> = true>
    DivNode<ValueT> operator/(T<ValueT> a, U<ValueT> b) {
        return DivNode<ValueT>(a, b);
    }

    namespace Visitor {
        template<typename ValueT>
        inline SharedNode<ValueT> eval_visitor(SharedNode<ValueT> nd) {
            using std::dynamic_pointer_cast;
            if (nd->type_ == Num) {
                return nd;
            } else if (nd->type_ == Add || nd->type_ == Sub || nd->type_ == Mul || nd->type_ == Div) {
                // Binary arithmetic
                auto op_nd = dynamic_pointer_cast<ArithmeticNode<ValueT>>(nd);
                op_nd->children_[0] = eval_visitor(op_nd->children_[0]);
                op_nd->children_[1] = eval_visitor(op_nd->children_[1]);
                if (op_nd->children_[0]->type_ == Num && op_nd->children_[1]->type_ == Num) {
                    ValueT l_val = dynamic_pointer_cast<NumericNode<ValueT>>(op_nd->children_[0])->val_;
                    ValueT r_val = dynamic_pointer_cast<NumericNode<ValueT>>(op_nd->children_[1])->val_;
                    switch (op_nd->type_) {
                        case Add:
                            return SharedNode<ValueT>(new NumericNode<ValueT>(l_val + r_val));
                        case Sub:
                            return SharedNode<ValueT>(new NumericNode<ValueT>(l_val - r_val));
                        case Mul:
                            return SharedNode<ValueT>(new NumericNode<ValueT>(l_val * r_val));
                        case Div:
                            return SharedNode<ValueT>(new NumericNode<ValueT>(l_val / r_val));
                        default: // Impossible!
                            break;
                    }
                }
            }
            // All unsupported op fall through here
            return nd;
        }

        template<typename ValueT, template<class> class NodeT,
                std::enable_if_t<std::is_base_of_v<NodeBase<ValueT>, NodeT<ValueT>>, bool> = true>
        inline SharedNode<ValueT> eval_visitor(const NodeT<ValueT> &nd) {
            return eval_visitor(SharedNode<ValueT>(new NodeT<ValueT>(nd)));
        }
    }
}

#endif //SPP_EXPRESSION_H
