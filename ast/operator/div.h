#ifndef SPP_AST_OPERATOR_DIV_H
#define SPP_AST_OPERATOR_DIV_H

#include <functional>

#include "base.h"
namespace Spp::__Ast {

inline const uint64_t DIV_OP_HASH_CODE = std::hash<std::string>{}(__FILE__);

class DivOp : public OperatorBase {
 public:
  template <typename T, typename U>
  requires is_unique_node<T> && is_unique_node<U> DivOp(T &&l, U &&r)
      : OperatorBase("/", 2, PosType::infix, std::move(l), std::move(r)){};

  UniqueNode simplify(UniqueNode &&self) override;

  uint64_t hash_code() const override;

  UniqueNode deep_copy() const override;
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERATOR_DIV_H
