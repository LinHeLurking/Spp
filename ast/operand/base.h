#ifndef SPP_AST_OPERAND_BASE_H
#define SPP_AST_OPERAND_BASE_H

#include <limits>

#include "../node.h"

namespace Spp::__Ast {
class OperandBase : public Node {
 public:
  uint priority() const override { return std::numeric_limits<uint>::max(); }

  UniqueNode expand_add(UniqueNode &&self) override { return std::move(self); }
};

}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERAND_BASE_H
