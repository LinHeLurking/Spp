#ifndef SPP_AST_OPERATOR_ADD_H
#define SPP_AST_OPERATOR_ADD_H

#include "../operand/number.h"
#include "base.h"

namespace Spp::__Ast {
class AddOp : public OperatorBase {
 public:
  // Binary add.
  template <typename T, typename U>
  requires is_unique_node<T> && is_unique_node<U> AddOp(T&& l, U&& r)
      : OperatorBase("+", 1, PosType::infix, std::move(l), std::move(r)) {}

  // Multiple add.
  template <typename RandIt>
  // Only takes move iterator
  requires std::is_same_v<UniqueNode&&, decltype(*(std::declval<RandIt>()))> &&
      requires {
    // Only take random access iterator(which can add and compare).
    std::declval<RandIt>()++;
    std::declval<RandIt>() == std::declval<RandIt>();
    std::declval<RandIt>() != std::declval<RandIt>();
  }
  AddOp(RandIt begin, RandIt end)
      : OperatorBase("+", 1, PosType::infix, begin, end) {}

  UniqueNode eval(UniqueNode&& self) override {
    assert(self.get() == this);
    eval_sub_tree();
    if (all_child_num()) {
      // auto [l, r] = get_num_unchecked<2>();
      auto child_num = get_num_unchecked(child_.size());
      SmartNum val{0};
      for (auto& num : child_num) {
        val = val + num;
      }
      return UniqueNode(new Number(val));
    }
    return std::move(self);
  }

  UniqueNode deep_copy() const override {
    return UniqueNode(
        new AddOp(child_[0]->deep_copy(), child_[1]->deep_copy()));
  }
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERATOR_ADD_H
