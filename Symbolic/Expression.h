#ifndef SPP_EXPRESSION_H
#define SPP_EXPRESSION_H

#include "cstdint"
#include <functional>
#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <variant>
#include <map>

#include "Number.h"

namespace Spp {

    using std::dynamic_pointer_cast;
    using std::static_pointer_cast;
    using std::make_shared;
    using std::string;
    using std::map;

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
            explicit ArithmeticNode(ExprNodeType type, int priority, string name,
                                    SharedNode left, SharedNode right)
                    : name_(name), NodeBase(type, priority),
                      left_(std::move(left)), right_(std::move(right)) {}

            const string name_;
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

        SharedNode add(const SharedNode &a, const SharedNode &b) {
            return static_pointer_cast<NodeBase>(make_shared<AddNode>(a, b));
        }

        SharedNode sub(const SharedNode &a, const SharedNode &b) {
            return static_pointer_cast<NodeBase>(make_shared<SubNode>(a, b));
        }

        SharedNode mul(const SharedNode &a, const SharedNode &b) {
            return static_pointer_cast<NodeBase>(make_shared<MulNode>(a, b));
        }

        SharedNode div(const SharedNode &a, const SharedNode &b) {
            return static_pointer_cast<NodeBase>(make_shared<DivNode>(a, b));
        }

        class VarNode : public NodeBase {
        public:
            explicit VarNode(const char *name)
                    : name_(name), NodeBase(Var, LN_OP) {}

            explicit VarNode(string name)
                    : name_(name), NodeBase(Var, LN_OP) {}

            string name_;
        };


        inline string wrap_bracket(const string &s) {
            return "(" + s + ")";
        }

        inline string to_string(const SharedNode &nd) {
            using Numeric::to_string;
            switch (nd->type_) {
                case ExprImpl::Num: {
                    auto cur = static_pointer_cast<NumericNode>(nd);
                    return to_string(cur->val_);
                }
                case ExprImpl::Var: {
                    auto cur = static_pointer_cast<VarNode>(nd);
                    return cur->name_;
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

        inline SharedNode eval(SharedNode nd, const map<string, SharedNode> &mp) {
            if (nd->type_ == Var) {
                auto cur = static_pointer_cast<VarNode>(nd);
                if (mp.find(cur->name_) != mp.end()) {
                    return mp.at(cur->name_);
                } else {
                    return nd;
                }
            } else if (nd->type_ == Add || nd->type_ == Sub || nd->type_ == Mul || nd->type_ == Div) {
                auto cur = static_pointer_cast<ArithmeticNode>(nd);
                auto l_ptr = eval(cur->left_, mp);
                auto r_ptr = eval(cur->right_, mp);
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

//        inline SharedNode eval(SharedNode nd, const map<string, Numeric::Number> &mp) {
//            map<string, SharedNode> real_map;
//            for (const auto&[k, v]: mp) {
//                real_map[k] = make_shared<NumericNode>(v);
//            }
//            return eval(nd, real_map);
//        }

        inline SharedNode copy_tree(const SharedNode &nd) {
            if (nd->type_ == Num) {
                auto cur = static_pointer_cast<NumericNode>(nd);
                return make_shared<NumericNode>(cur->val_);
            } else if (nd->type_ == Var) {
                auto cur = static_pointer_cast<VarNode>(nd);
                return make_shared<VarNode>(cur->name_);
            } else { // Arithmetic
                auto cur = static_pointer_cast<ArithmeticNode>(nd);
                SharedNode left = copy_tree(cur->left_);
                SharedNode right = copy_tree(cur->right_);
                return make_shared<ArithmeticNode>(cur->type_, cur->priority_, cur->name_, left, right);
            }
        }

        inline SharedNode substitute(SharedNode nd, const map<string, SharedNode> &mp) {
            if (nd->type_ == Var) {
                auto cur = static_pointer_cast<VarNode>(nd);
                if (mp.find(cur->name_) != mp.end()) {
                    return mp.at(cur->name_);
                } else {
                    return nd;
                }
            } else if (nd->type_ == Num) {
                auto cur = static_pointer_cast<NumericNode>(nd);
                return make_shared<NumericNode>(cur->val_);
            } else { // Arithmetic
                auto cur = static_pointer_cast<ArithmeticNode>(nd);
                auto left = substitute(cur->left_, mp);
                auto right = substitute(cur->right_, mp);
                return make_shared<ArithmeticNode>(cur->type_, cur->priority_, cur->name_, left, right);
            }
        }
    }

    using std::make_shared;
    using ExprImpl::NumericNode;
    using ExprImpl::VarNode;
    using ExprImpl::SharedNode;


    class Expr {
    public:
        template<typename ValueT>
        Expr(ValueT val) : nd_(make_shared<NumericNode>(val)) {
            static_assert(std::is_same_v<ValueT, Numeric::Number> || Numeric::is_numeric_v<ValueT>);
        }

        Expr(const Expr &other) : nd_(ExprImpl::copy_tree(other.nd_)) {}

        Expr &operator=(const Expr &other) {
            if (&other == this) {
                return *this;
            }
            this->nd_ = ExprImpl::copy_tree(other.nd_);
            return *this;
        }

        inline SharedNode copy_tree() const {
            return ExprImpl::copy_tree(nd_);
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

        inline string to_string() const {
            return ExprImpl::to_string(nd_);
        }

        inline Expr eval() const {
            return Expr(ExprImpl::eval(nd_));
        }

        inline Expr eval(const map<string, Expr> &mp) const {
            auto real_mp = expr_mp_to_nd_mp(mp);
            return Expr(ExprImpl::eval(nd_, real_mp));
        }

        inline Expr substitute(const map<string, Expr> &mp) const {
            auto real_mp = expr_mp_to_nd_mp(mp);
            return Expr(ExprImpl::substitute(nd_, real_mp));
        }

        friend std::ostream &operator<<(std::ostream &os, const Expr &term) {
            os << term.to_string();
            return os;
        }

    protected:
        ExprImpl::SharedNode nd_;

        explicit Expr(SharedNode nd) : nd_(std::move(nd)) {}

        explicit Expr(const char *var_name) : nd_(make_shared<VarNode>(var_name)) {}

        static inline map<string, SharedNode> expr_mp_to_nd_mp(const map<string, Expr> &mp) {
            map<string, SharedNode> real_mp;
            for (const auto&[k, v]: mp) {
                real_mp[k] = v.copy_tree();
            }
            return real_mp;
        }
    };

    class Var : public Expr {
    public:
        explicit Var(const char *name) : Expr(name) {}
    };

    using Constant = Expr;
}

#endif //SPP_EXPRESSION_H
