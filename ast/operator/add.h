#ifndef SPP_AST_OPERATOR_ADD_H
#define SPP_AST_OPERATOR_ADD_H

#include "base.h"

namespace Spp::__Ast {
class AddOp : public OperatorBase {
 public:
  template <typename T, typename U>
  requires is_unique_node<T> && is_unique_node<U> AddOp(T &&l, U &&r)
      : OperatorBase("+", 1, PosType::infix, std::move(l), std::move(r)) {}
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERATOR_ADD_H
