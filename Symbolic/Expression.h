#ifndef SPP_EXPRESSION_H
#define SPP_EXPRESSION_H

#include "cstdint"
#include <functional>
#include <vector>
#include <string>
#include <memory>

#include "Number.h"

namespace Spp::Expression {

    template<typename ValueT>
    struct OperatorFunc {
        const std::string name_;
        std::function<ValueT(const std::vector<ValueT> &)> func_;

        inline ValueT operator()(const std::vector<ValueT> &v) {
            return func_(v);
        }
    };

    template<typename ValueT>
    inline auto OP_ADD = OperatorFunc<ValueT>{
            .name_="+",
            .func_=[](const std::vector<ValueT> &v) {
                return v[0] + v[1];
            }};

    template<typename ValueT, std::enable_if_t<Numeric::is_numeric_v < ValueT>, bool> = true>

    class NodeBase {
    public:
        // For numeric nodes, just returning is enough.
        // For op nodes, evaluation over the sub-tree.
        // Values will be cached.
        virtual ValueT val() = 0;
    };

    template<typename ValueT>
    using SharedNode = std::shared_ptr<NodeBase<ValueT>>;


    template<typename ValueT>
    class NumericNode : public NodeBase<ValueT> {
    public:
        ValueT val() override {
            return val_;
        }

        explicit NumericNode(ValueT val) : val_(val) {}

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
                return this->cached_val_ = op_(child_val);
            }
        }

        explicit OperatorNode(OperatorFunc<ValueT> op) : op_(op) {}

        std::vector<SharedNode<ValueT>> children_;
        OperatorFunc<ValueT> op_;

    protected:
        bool cache_initialized_ = false;
        ValueT cached_val_ = static_cast<ValueT>(0);
    };

    template<typename ValueT>
    class AddNode : public OperatorNode<ValueT> {
    public:
        template<typename T, typename U>
        explicit AddNode(T a, U b):OperatorNode<ValueT>(OP_ADD<ValueT>) {
            this->children_.push_back(SharedNode<ValueT>(new T(a)));
            this->children_.push_back(SharedNode<ValueT>(new U(b)));
        }
    };

    template<typename ValueT, template<class ...> class T,
            template<class ...> class U>
    AddNode<ValueT> operator+(T<ValueT> a, U<ValueT> b) {
        return AddNode<ValueT>(a, b);
    }


    template<typename ValueT, typename ChildValueT>
    class CastNode : public NodeBase<ValueT> {
    public:
        ValueT val() override {
            return static_cast<ValueT>(child_->val());
        }

        template<template<class ...> class ChildNodeT>
        explicit CastNode(ChildNodeT<ChildValueT> child) {
            child_ = SharedNode<ChildValueT>(new ChildNodeT<ChildValueT>(child));
        }

        SharedNode<ChildValueT> child_;
    };
}

#endif //SPP_EXPRESSION_H
