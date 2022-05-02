#ifndef SPP_AST_OPERATOR_NEG_H
#define SPP_AST_OPERATOR_NEG_H

#include "base.h"
namespace Spp::__Ast {

inline const uint64_t NEG_OP_HASH_CODE = std::hash<std::string>{}(__FILE__);

class NegOp : public OperatorBase {
 public:
  template <typename T>
  requires is_unique_node<T> NegOp(T &&sub)
      : OperatorBase("-", 0, PosType::prefix_op, std::move(sub)) {}

  UniqueNode simplify(UniqueNode &&self) override {
    assert(self.get() == this);
    simplify_sub_tree();
    if (all_child_num()) {
      auto [sub] = get_num_unchecked<1>();
      return UniqueNode(new Number(-sub));
    }
    return std::move(self);
  }

  uint64_t hash_code() const override {
    return NEG_OP_HASH_CODE ^ (combine_child_hash() << 1);
  }

  UniqueNode deep_copy() const override {
    return UniqueNode(new NegOp(child_[0]->deep_copy()));
  }
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERATOR_NEG_H
