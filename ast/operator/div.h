#ifndef AST_OPERATOR_DIV_H
#define AST_OPERATOR_DIV_H

#include "base.h"
namespace Spp::__Ast {
class DivOp : public OperatorBase {
 public:
  DivOp(UniqueNode &l, UniqueNode &r)
      : OperatorBase("/", 2, PosType::infix, std::move(l), std::move(r)){};
};
}  // namespace Spp::__Ast

#endif  // !AST_OPERATOR_DIV_H
