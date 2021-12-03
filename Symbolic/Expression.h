#ifndef SPP_EXPRESSION_H
#define SPP_EXPRESSION_H

#include "cstdint"
#include <functional>
#include <vector>
#include <string>
#include <memory>

#include "Number.h"

namespace Spp::Expression {

    // L1: +, -
    // L2: *, /
    // L3: ^
    // LN_OP: Numeric
    constexpr int L1_OP = 1, L2_OP = 2, L3_OP = 3, LN_OP = 10000;

    template<typename ValueT, std::enable_if_t<Numeric::is_numeric_v < ValueT>, bool> = true>

    class NodeBase {
    public:
        // For numeric nodes, just returning is enough.
        // For op nodes, evaluation over the sub-tree.
        // Values will be cached.
        virtual ValueT val() = 0;

        int pred_;

        explicit NodeBase(int pred) : pred_(pred) {}

        virtual std::string expr_to_string() const = 0;
    };

    template<typename ValueT>
    using SharedNode = std::shared_ptr<NodeBase<ValueT>>;

    std::string wrap_bracket(const std::string &s) {
        return "(" + s + ")";
    }


    template<typename ValueT>
    class NumericNode : public NodeBase<ValueT> {
    public:
        ValueT val() override {
            return val_;
        }

        explicit NumericNode(ValueT val) : val_(val), NodeBase<ValueT>(LN_OP) {}

        std::string expr_to_string() const override {
            return to_string(val_);
        }

    protected:
        ValueT val_;
    };

    template<typename ValueT>
    class OperatorNode : public NodeBase<ValueT> {
    public:
        inline ValueT val() override {
            if (this->cache_initialized_) {
                return this->cached_val_;
            } else {
                std::vector<ValueT> child_val;
                for (auto child: children_) {
                    child_val.push_back(child->val());
                }
                this->cache_initialized_ = true;
                return this->cached_val_ = op(child_val);
            }
        }

        template<class StringT>
        explicit OperatorNode(int pred, StringT name) :NodeBase<ValueT>(pred), name_(name) {}

        virtual ValueT op(const std::vector<ValueT> &v) = 0;

        const std::string name_;

        std::vector<SharedNode<ValueT>> children_;

    protected:
        bool cache_initialized_ = false;
        ValueT cached_val_ = static_cast<ValueT>(0);
    };

    template<typename ValueT>
    class BinaryOpNode : public OperatorNode<ValueT> {
    public:
        template<typename T, typename U>
        explicit BinaryOpNode(T a, U b, int pred, std::string name) : OperatorNode<ValueT>(pred, name) {
            this->children_.push_back(SharedNode<ValueT>(new T(a)));
            this->children_.push_back(SharedNode<ValueT>(new U(b)));
        }

        std::string expr_to_string() const override {
            auto left = this->children_[0];
            auto right = this->children_[1];
            auto l_str = left->pred_ < this->pred_ ? wrap_bracket(left->expr_to_string()) : left->expr_to_string();
            auto r_str = right->pred_ < this->pred_ ? wrap_bracket(right->expr_to_string()) : right->expr_to_string();
            return l_str + " " + this->name_ + " " + r_str;
        }
    };

    template<typename ValueT>
    class AddNode : public BinaryOpNode<ValueT> {
    public:
        template<typename T, typename U>
        explicit AddNode(T a, U b):BinaryOpNode<ValueT>(a, b, L1_OP, "+") {}

        ValueT op(const std::vector<ValueT> &v) override {
            return v[0] + v[1];
        }
    };

    template<typename ValueT>
    class SubNode : public BinaryOpNode<ValueT> {
    public:
        template<typename T, typename U>
        explicit SubNode(T a, U b):BinaryOpNode<ValueT>(a, b, L1_OP, "-") {}

        ValueT op(const std::vector<ValueT> &v) override {
            return v[0] - v[1];
        }
    };

    template<typename ValueT>
    class MulNode : public BinaryOpNode<ValueT> {
    public:
        template<typename T, typename U>
        explicit MulNode(T a, U b):BinaryOpNode<ValueT>(a, b, L2_OP, "*") {}

        ValueT op(const std::vector<ValueT> &v) override {
            return v[0] * v[1];
        }
    };

    template<typename ValueT>
    class DivNode : public BinaryOpNode<ValueT> {
    public:
        template<typename T, typename U>
        explicit DivNode(T a, U b):BinaryOpNode<ValueT>(a, b, L2_OP, "/") {}

        ValueT op(const std::vector<ValueT> &v) override {
            return v[0] / v[1];
        }
    };

    template<typename ValueT, template<class ...> class T,
            template<class ...> class U>
    AddNode<ValueT> operator+(T<ValueT> a, U<ValueT> b) {
        return AddNode<ValueT>(a, b);
    }

    template<typename ValueT, template<class ...> class T,
            template<class ...> class U>
    SubNode<ValueT> operator-(T<ValueT> a, U<ValueT> b) {
        return SubNode<ValueT>(a, b);
    }

    template<typename ValueT, template<class ...> class T,
            template<class ...> class U>
    MulNode<ValueT> operator*(T<ValueT> a, U<ValueT> b) {
        return MulNode<ValueT>(a, b);
    }

    template<typename ValueT, template<class ...> class T,
            template<class ...> class U>
    DivNode<ValueT> operator/(T<ValueT> a, U<ValueT> b) {
        return DivNode<ValueT>(a, b);
    }


    template<typename ValueT, typename ChildValueT>
    class CastNode : public NodeBase<ValueT> {
    public:
        ValueT val() override {
            return static_cast<ValueT>(child_->val());
        }

        std::string expr_to_string() const override {
            return Numeric::to_string(static_cast<ValueT>(child_->val()));
        }

        template<template<class ...> class ChildNodeT>
        explicit CastNode(ChildNodeT<ChildValueT> child):NodeBase<ValueT>(child.pred_) {
            child_ = SharedNode<ChildValueT>(new ChildNodeT<ChildValueT>(child));
        }

        SharedNode<ChildValueT> child_;
    };
}

#endif //SPP_EXPRESSION_H
