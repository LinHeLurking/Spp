#ifndef AST_OPERATOR_MUL_H
#define AST_OPERATOR_MUL_H

#include "base.h"
namespace Spp::__Ast {
class MulOp : public OperatorBase {
 public:
  MulOp(UniqueNode &l, UniqueNode &r)
      : OperatorBase("*", 2, PosType::infix, std::move(l), std::move(r)) {}
};
}  // namespace Spp::__Ast

#endif  // !AST_OPERATOR_MUL_H
