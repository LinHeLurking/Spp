#include "sub.h"

#include "add.h"
#include "base.h"
#include "neg.h"

namespace Spp::__Ast {

UniqueNode SubOp::simplify(UniqueNode &&self) {
  assert(self.get() == this);
  simplify_sub_tree();
  if (all_child_num()) {
    auto [l, r] = get_child_num_unchecked<2>();
    return UniqueNode(new Number(l - r));
  }
  return std::move(self);
}

UniqueNode SubOp::expand_add(UniqueNode &&self) {
  assert(this == self.get());
  expand_add_sub_tree();
  std::vector<UniqueNode> alt;
  OperatorBase *sub;
  if (child_[0]->tag() == NodeTag::Operator &&
      "+" == (sub = static_cast<OperatorBase *>(child_[0].get()))->name()) {
    for (auto it = sub->child_.begin(); it != sub->child_.end(); ++it) {
      alt.emplace_back(std::move(*it));
    }
  } else {
    alt.emplace_back(std::move(child_[0]));
  }
  if (child_[1]->tag() == NodeTag::Operator &&
      "+" == (sub = static_cast<OperatorBase *>(child_[1].get()))->name()) {
    for (auto it = sub->child_.begin(); it != sub->child_.end(); ++it) {
      alt.emplace_back(new NegOp(std::move(*it)));
    }
  } else {
    alt.emplace_back(new NegOp(std::move(child_[1])));
  }
  return UniqueNode(new AddOp(alt.begin(), alt.end()));
}

uint64_t SubOp::hash_code() const {
  return SUB_OP_HASH_CODE ^ (combine_child_hash() << 1);
}

UniqueNode SubOp::deep_copy() const {
  return UniqueNode(new SubOp(child_[0]->deep_copy(), child_[1]->deep_copy()));
}

}  // namespace Spp::__Ast
