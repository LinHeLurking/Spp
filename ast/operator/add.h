#ifndef SPP_AST_OPERATOR_ADD_H
#define SPP_AST_OPERATOR_ADD_H

#include <cassert>
#include <functional>
#include <string>

#include "../operand/number.h"
#include "base.h"
#include "neg.h"

namespace Spp::__Ast {

// Use inline to declare variable in header files.
inline uint64_t ADD_OP_HASH_SEED = std::hash<std::string>{}(__FILE__);

class AddOp : public OperatorBase {
 public:
  // Binary add.
  template <typename T, typename U>
  requires is_unique_node<T> && is_unique_node<U> AddOp(T&& l, U&& r)
      : OperatorBase("+", 1, PosType::infix, std::move(l), std::move(r)) {}

  // Multiple add.
  template <typename RandIt>
  // Only takes move iterator
  requires std::is_same_v<UniqueNode,
                          std::decay_t<decltype(*(std::declval<RandIt>()))>> &&
      requires {
    // Only take random access iterator(which can add and compare).
    std::declval<RandIt>()++;
    std::declval<RandIt>() == std::declval<RandIt>();
    std::declval<RandIt>() != std::declval<RandIt>();
  }
  AddOp(RandIt&& begin, RandIt&& end)
      : OperatorBase("+", 1, PosType::infix, std::forward<RandIt>(begin),
                     std::forward<RandIt>(end)) {}

  UniqueNode eval(UniqueNode&& self) override {
    assert(self.get() == this);
    eval_sub_tree();
    if (all_child_num()) {
      // auto [l, r] = get_num_unchecked<2>();
      auto child_num = get_num_unchecked();
      SmartNum val{0};
      for (auto& num : child_num) {
        val = val + num;
      }
      return UniqueNode(new Number(val));
    }
    return std::move(self);
  }

  UniqueNode expand_add(UniqueNode&& self) override {
    assert(this == self.get());
    expand_add_sub_tree();
    std::vector<UniqueNode> alt;
    alt.reserve(child_.size());
    for (auto it = child_.rbegin(); it != child_.rend(); ++it) {
      alt.emplace_back(std::move(*it));
    }
    child_.clear();
    for (auto it = alt.rbegin(); it != alt.rend(); ++it) {
      if ((*it)->tag() == NodeTag::Operator) {
        auto x = static_cast<OperatorBase*>(it->get());
        if (x->name() == "+") {
          for (auto jt = x->child_.rbegin(); jt != x->child_.rend(); ++jt) {
            child_.emplace_back(std::move(*jt));
          }
        }
        // After child expand_add, (a-b) will be expanded into (a+(-b)).
        // So, there is no need to specially consider SubOp.

        // else if (x->name() == "-" && x->child_.size() == 2) {
        //       // SubOp
        // child_.emplace_back(std::move(x->child_[0]));
        // child_.emplace_back(new NegOp(std::move(x->child_[1])));
        // }
        else {
          child_.emplace_back(std::move(*it));
        }
      } else {
        child_.emplace_back(std::move(*it));
      }
    }
    return std::move(self);
  }

  uint64_t hash_code() const override {
    return ADD_OP_HASH_SEED ^ (combine_child_hash() << 1);
  }

  UniqueNode deep_copy() const override {
    return UniqueNode(
        new AddOp(child_[0]->deep_copy(), child_[1]->deep_copy()));
  }
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERATOR_ADD_H
