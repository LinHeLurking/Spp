#ifndef AST_OPERAND_BASE_H
#define AST_OPERAND_BASE_H

#include <limits>

#include "../node.h"

namespace Spp::__Ast {
class OperandBase : public Node {
 public:
  uint priority() const override { return std::numeric_limits<uint>::max(); }
};

}  // namespace Spp::__Ast

#endif  // !AST_OPERAND_BASE_H
