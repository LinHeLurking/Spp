#ifndef SPP_AST_OPERATOR_MUL_H
#define SPP_AST_OPERATOR_MUL_H

#include <vector>

#include "add.h"
#include "base.h"

namespace Spp::__Ast {
class MulOp : public OperatorBase {
 public:
  template <typename T, typename U>
  requires is_unique_node<T> && is_unique_node<U> MulOp(T &&l, U &&r)
      : OperatorBase("*", 2, PosType::infix, std::move(l), std::move(r)) {}

  UniqueNode eval(UniqueNode &&self) override {
    assert(self.get() == this);
    eval_sub_tree();
    if (all_child_num()) {
      auto [l, r] = get_num_unchecked<2>();
      return UniqueNode(new Number(l * r));
    }
    return std::move(self);
  }

  UniqueNode expand_add(UniqueNode &&self) override {
    expand_add_sub_tree();
    std::vector<UniqueNode> l, r, child;
    auto take = [&](UniqueNode &from, std::vector<UniqueNode> &to) {
      if (from->tag() != NodeTag::Operator) {
        to.push_back(std::move(from));
      } else {
        auto tmp = static_cast<OperatorBase *>(from.get());
        for (auto &x : tmp->child_) {
          to.push_back(std::move(x));
        }
      }
    };
    take(child_[0], l);
    take(child_[1], r);
    for (auto &x : l) {
      for (auto &y : r) {
        // TODO: Actually some copy can be saved.
        child.push_back(UniqueNode(new MulOp(x->deep_copy(), y->deep_copy())));
      }
    }
    std::move_iterator begin{child.begin()}, end{child.end()};
    return UniqueNode(new AddOp(begin, end));
  }

  UniqueNode deep_copy() const override {
    return UniqueNode(
        new MulOp(child_[0]->deep_copy(), child_[1]->deep_copy()));
  }
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERATOR_MUL_H
