#ifndef AST_OPERATOR_SUB_H
#define AST_OPERATOR_SUB_H

#include "base.h"

namespace Spp::__Ast {
class SubOp : public OperatorBase {
 public:
  SubOp(UniqueNode &l, UniqueNode &r)
      : OperatorBase("-", 1, PosType::infix, std::move(l), std::move(r)){};
};
}  // namespace Spp::__Ast
#endif  // !AST_OPERATOR_NEG_H
