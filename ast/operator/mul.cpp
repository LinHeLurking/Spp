#include "mul.h"

#include "add.h"

namespace Spp::__Ast {

UniqueNode MulOp::simplify(UniqueNode &&self) {
  assert(self.get() == this);
  simplify_sub_tree();
  if (all_child_num()) {
    auto [l, r] = get_child_num_unchecked<2>();
    return UniqueNode(new Number(l * r));
  }
  return std::move(self);
}

UniqueNode MulOp::expand_add(UniqueNode &&self) {
  expand_add_sub_tree();
  std::vector<UniqueNode> l, r, child;
  auto take = [&](UniqueNode &from, std::vector<UniqueNode> &to) {
    if (from->tag() != NodeTag::Operator) {
      to.emplace_back(std::move(from));
    } else {
      auto tmp = static_cast<OperatorBase *>(from.get());
      for (auto &x : tmp->child_) {
        to.emplace_back(std::move(x));
      }
    }
  };
  take(child_[0], l);
  take(child_[1], r);
  for (auto &x : l) {
    for (auto &y : r) {
      // TODO: Actually some copy can be saved.
      child.emplace_back(new MulOp(x->deep_copy(), y->deep_copy()));
    }
  }
  std::move_iterator begin{child.begin()}, end{child.end()};
  return UniqueNode(new AddOp(begin, end));
}

uint64_t MulOp::hash_code() const {
  return MUL_OP_HASH_CODE ^ (combine_child_hash() << 1);
}

UniqueNode MulOp::deep_copy() const {
  return UniqueNode(new MulOp(child_[0]->deep_copy(), child_[1]->deep_copy()));
}

}  // namespace Spp::__Ast
