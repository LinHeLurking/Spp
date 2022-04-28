#ifndef SPP_AST_OPERATOR_MUL_H
#define SPP_AST_OPERATOR_MUL_H

#include "base.h"
namespace Spp::__Ast {
class MulOp : public OperatorBase {
 public:
  template <typename T, typename U>
  requires is_unique_node<T> && is_unique_node<U> MulOp(T &&l, U &&r)
      : OperatorBase("*", 2, PosType::infix, std::move(l), std::move(r)) {}
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERATOR_MUL_H
