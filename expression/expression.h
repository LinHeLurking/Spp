#ifndef SPP_EXPRESSION_H
#define SPP_EXPRESSION_H

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>

#include "../ast/ast.h"
#include "../util/concept.h"

namespace Spp::__Expression {

using Ast = Spp::__Ast::UniqueNode;
using Asts = Spp::__Ast::UniqueNodes;
using NegOp = Spp::__Ast::NegOp;
using AddOp = Spp::__Ast::AddOp;
using SubOp = Spp::__Ast::SubOp;
using MulOp = Spp::__Ast::MulOp;
using DivOp = Spp::__Ast::DivOp;
using Spp::__Concept::SignedInteger;
using Spp::__Concept::UnsignedInteger;

template <typename T, typename U>
inline constexpr bool is_self_or_ref =
    std::is_same_v<std::decay_t<U>, std::decay_t<T>>;

class Expression {
 public:
  Expression(Expression &&expr) : ast_(std::move(expr.ast_)) {}
  Expression(const Expression &expr) : ast_(expr.ast_->deep_copy()) {
#ifndef NDEBUG
    // This is used to inform protential optimizations.
    // In most cases, such copies are redundant.
    std::cout << "Expression copy construct" << std::endl;
#endif
  }

  Expression &&operator=(Expression &&expr) {
    ast_ = std::move(expr.ast_);
    return std::move(*this);
  }

  Expression &&operator=(const Expression &expr) {
#ifndef NDEBUG
    std::cout << "Expression copy assignment" << std::endl;
#endif
    ast_ = expr.ast_->deep_copy();
    return std::move(*this);
  }

  template <typename T>
  requires SignedInteger<T> || std::is_floating_point_v<T>
  explicit Expression(T x) : ast_(Asts::number(x)) {}

  // Question: Is it needed to construct rational number directly?
  // Because we have operator/ already.

  // template<typename T>
  // requires SignedInteger<T>
  // Expression(T n):ast_()

  /**
   * These template operators are used to automatically support
   * both copy and move with as few copies as possible. Some examples:
   * auto z = x + y;
   * auto z = x + std::move(y);
   * auto z = std::move(x) + std::move(y);
   * auto z = std::move(x) + y;
   */

  template <typename T>
  requires is_self_or_ref<T, Expression>
  friend inline Expression operator-(T &&expr) {
    return {Ast(new NegOp(take_ast(expr)))};
  }

#define GEN_BIN_OP(op_node_name, op_func_name)                            \
  template <typename T, typename U>                                       \
  requires is_self_or_ref<T, Expression> && is_self_or_ref<U, Expression> \
  friend inline Expression op_func_name(T &&lhs, U &&rhs) {               \
    return {Ast(new op_node_name(take_ast(lhs), take_ast(rhs)))};         \
  }

  GEN_BIN_OP(AddOp, operator+);
  GEN_BIN_OP(SubOp, operator-);
  GEN_BIN_OP(MulOp, operator*);
  GEN_BIN_OP(DivOp, operator/);

#undef GEN_BIN_OP

  /**
   * I/O member functions.
   */

  std::string to_string() const { return ast_->to_string(); }

  friend inline std::ostream &operator<<(std::ostream &os,
                                         const Expression &expr) {
    os << expr.to_string();
    return os;
  }

  /**
   * Evaluate this.
   */
  Expression &&eval() {
    ast_ = std::move(ast_->eval(std::move(ast_)));
    return std::move(*this);
  }

  /**
   * Expand all add operations. (a+b)*(c+d) => ac + ad + bc + bd
   */
  Expression &&expand_add() {
    ast_ = std::move(ast_->expand_add(std::move(ast_)));
    return std::move(*this);
  }

 private:
  Ast ast_;

  Expression(Ast &ast) : ast_(std::move(ast)) {}
  Expression(Ast &&ast) : ast_(std::move(ast)) {}

  static inline Ast take_ast(Expression &&expr) { return std::move(expr.ast_); }

  static inline Ast take_ast(Expression &expr) {
    return expr.ast_->deep_copy();
  }
};
}  // namespace Spp::__Expression

namespace Spp {
using Expression = __Expression::Expression;
}  // namespace Spp

#endif  // !SPP_EXPRESSION_H
