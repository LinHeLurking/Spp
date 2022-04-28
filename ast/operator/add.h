#ifndef AST_OPERATOR_ADD_H
#define AST_OPERATOR_ADD_H

#include "base.h"

namespace Spp::__Ast {
class AddOp : public OperatorBase {
 public:
  AddOp(UniqueNode &l, UniqueNode &r)
      : OperatorBase("+", 1, PosType::infix, std::move(l), std::move(r)) {}
};
}  // namespace Spp::__Ast

#endif  // !AST_OPERATOR_ADD_H
