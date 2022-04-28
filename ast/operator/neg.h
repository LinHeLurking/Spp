#ifndef AST_OPERATOR_NEG_H
#define AST_OPERATOR_NEG_H

#include "base.h"
namespace Spp::__Ast {
class NegOp : public OperatorBase {
 public:
  NegOp(UniqueNode &sub)
      : OperatorBase("-", 0, PosType::prefix_op, std::move(sub)) {}
};
}  // namespace Spp::__Ast

#endif  // !AST_OPERATOR_NEG_H
