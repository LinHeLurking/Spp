#ifndef SPP_AST_OPERATOR_MUL_H
#define SPP_AST_OPERATOR_MUL_H

#include <functional>
#include <vector>

#include "base.h"

namespace Spp::__Ast {

inline const uint64_t MUL_OP_HASH_CODE = std::hash<std::string>{}(__FILE__);

class MulOp : public OperatorBase {
 public:
  template <typename T, typename U>
  requires is_unique_node<T> && is_unique_node<U> MulOp(T &&l, U &&r)
      : OperatorBase("*", 2, PosType::infix, std::move(l), std::move(r)) {}

  UniqueNode simplify(UniqueNode &&self) override;

  UniqueNode expand_add(UniqueNode &&self) override;

  uint64_t hash_code() const override;

  UniqueNode deep_copy() const override;
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERATOR_MUL_H
