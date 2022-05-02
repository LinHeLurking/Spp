#include "div.h"

#include "base.h"

namespace Spp::__Ast {

UniqueNode DivOp::simplify(UniqueNode &&self) {
  assert(self.get() == this);
  simplify_sub_tree();
  if (all_child_num()) {
    auto [l, r] = get_child_num_unchecked<2>();
    return UniqueNode(new Number(l / r));
  }
  return std::move(self);
}

uint64_t DivOp::hash_code() const {
  return DIV_OP_HASH_CODE ^ (combine_child_hash() << 1);
}

UniqueNode DivOp::deep_copy() const {
  return UniqueNode(new DivOp(child_[0]->deep_copy(), child_[1]->deep_copy()));
}

}  // namespace Spp::__Ast
