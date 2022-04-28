#ifndef SPP_AST_OPERATOR_DIV_H
#define SPP_AST_OPERATOR_DIV_H

#include "base.h"
namespace Spp::__Ast {
class DivOp : public OperatorBase {
 public:
  template <typename T, typename U>
  requires is_unique_node<T> && is_unique_node<U> DivOp(T &&l, U &&r)
      : OperatorBase("/", 2, PosType::infix, std::move(l), std::move(r)){};
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERATOR_DIV_H
