#ifndef SPP_AST_OPERATOR_NEG_H
#define SPP_AST_OPERATOR_NEG_H

#include "base.h"
namespace Spp::__Ast {
class NegOp : public OperatorBase {
 public:
  template <typename T>
  requires is_unique_node<T> NegOp(T &&sub)
      : OperatorBase("-", 0, PosType::prefix_op, std::move(sub)) {}

  UniqueNode eval(UniqueNode &&self) override {
    assert(self.get() == this);
    eval_sub_tree();
    if (child_[0]->is_number()) {
      auto [sub] = get_num_unchecked<1>();
      return UniqueNode(new Number(-sub));
    }
    return std::move(self);
  }

  UniqueNode deep_copy() const override {
    return UniqueNode(new NegOp(child_[0]->deep_copy()));
  }
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERATOR_NEG_H
