#ifndef SPP_AST_OPERATOR_NEG_H
#define SPP_AST_OPERATOR_NEG_H

#include <functional>

#include "base.h"

namespace Spp::__Ast {

inline const uint64_t NEG_OP_HASH_CODE = std::hash<std::string>{}(__FILE__);

class NegOp : public OperatorBase {
 public:
  template <typename T>
  requires is_unique_node<T> NegOp(T &&sub)
      : OperatorBase("-", 0, PosType::prefix_op, std::move(sub)) {}

  UniqueNode simplify(UniqueNode &&self) override;

  uint64_t hash_code() const override;

  UniqueNode deep_copy() const override;
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERATOR_NEG_H
