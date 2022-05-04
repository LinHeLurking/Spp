#ifndef SPP_AST_OPERAND_BASE_H
#define SPP_AST_OPERAND_BASE_H

#include <cassert>
#include <limits>

#include "../node.h"

namespace Spp::__Ast {
class OperandBase : public Node {
 public:
  uint32_t priority() const override;

  uint64_t size() const override;

  UniqueNode expand_add(UniqueNode &&self) override;

  UniqueNode collect(UniqueNode &&self, uint64_t &hash) override;
};

}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERAND_BASE_H
