#ifndef SPP_AST_OPERATOR_SUB_H
#define SPP_AST_OPERATOR_SUB_H

#include "add.h"
#include "base.h"
#include "neg.h"

namespace Spp::__Ast {

inline const uint64_t SUB_OP_HASH_CODE = std::hash<std::string>{}(__FILE__);

class SubOp : public OperatorBase {
 public:
  template <typename T, typename U>
  requires is_unique_node<T> && is_unique_node<U> SubOp(T &&l, U &&r)
      : OperatorBase("-", 1, PosType::infix, std::move(l), std::move(r)){};

  UniqueNode simplify(UniqueNode &&self) override {
    assert(self.get() == this);
    simplify_sub_tree();
    if (all_child_num()) {
      auto [l, r] = get_num_unchecked<2>();
      return UniqueNode(new Number(l - r));
    }
    return std::move(self);
  }

  UniqueNode expand_add(UniqueNode &&self) override {
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

  uint64_t hash_code() const override {
    return SUB_OP_HASH_CODE ^ (combine_child_hash() << 1);
  }

  UniqueNode deep_copy() const override {
    return UniqueNode(
        new SubOp(child_[0]->deep_copy(), child_[1]->deep_copy()));
  }
};
}  // namespace Spp::__Ast
#endif  // !SPP_AST_OPERATOR_NEG_H
