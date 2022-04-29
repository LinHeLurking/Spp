#ifndef SPP_AST_OPERATOR_SUB_H
#define SPP_AST_OPERATOR_SUB_H

#include "add.h"
#include "base.h"
#include "neg.h"

namespace Spp::__Ast {
class SubOp : public OperatorBase {
 public:
  template <typename T, typename U>
  requires is_unique_node<T> && is_unique_node<U> SubOp(T &&l, U &&r)
      : OperatorBase("-", 1, PosType::infix, std::move(l), std::move(r)){};

  UniqueNode eval(UniqueNode &&self) override {
    assert(self.get() == this);
    eval_sub_tree();
    if (all_child_num()) {
      auto [l, r] = get_num_unchecked<2>();
      return UniqueNode(new Number(l - r));
    }
    return std::move(self);
  }

  UniqueNode expand_add(UniqueNode &&self) override {
    expand_add_sub_tree();
    auto r = UniqueNode(new NegOp(std::move(child_[1])));
    return UniqueNode(new AddOp(std::move(child_[0]), std::move(r)));
  }

  UniqueNode deep_copy() const override {
    return UniqueNode(
        new SubOp(child_[0]->deep_copy(), child_[1]->deep_copy()));
  }
};
}  // namespace Spp::__Ast
#endif  // !SPP_AST_OPERATOR_NEG_H
