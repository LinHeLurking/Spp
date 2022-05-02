#include "neg.h"

#include "base.h"

namespace Spp::__Ast {

UniqueNode NegOp::simplify(UniqueNode &&self) {
  assert(self.get() == this);
  simplify_sub_tree();
  if (all_child_num()) {
    auto [sub] = get_child_num_unchecked<1>();
    return UniqueNode(new Number(-sub));
  }
  return std::move(self);
}

uint64_t NegOp::hash_code() const {
  return NEG_OP_HASH_CODE ^ (combine_child_hash() << 1);
}

UniqueNode NegOp::deep_copy() const {
  return UniqueNode(new NegOp(child_[0]->deep_copy()));
}

}  // namespace Spp::__Ast
