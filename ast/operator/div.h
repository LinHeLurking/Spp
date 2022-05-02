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

  UniqueNode simplify(UniqueNode &&self) override {
    assert(self.get() == this);
    simplify_sub_tree();
    if (all_child_num()) {
      auto [l, r] = get_child_num_unchecked<2>();
      return UniqueNode(new Number(l / r));
    }
    return std::move(self);
  }

  uint64_t hash_code() const override {
    return DIV_OP_HASH_CODE ^ (combine_child_hash() << 1);
  }

  UniqueNode deep_copy() const override {
    return UniqueNode(
        new DivOp(child_[0]->deep_copy(), child_[1]->deep_copy()));
  }
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERATOR_DIV_H
