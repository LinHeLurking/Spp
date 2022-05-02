#ifndef SPP_AST_OPERATOR_ADD_H
#define SPP_AST_OPERATOR_ADD_H

#include <functional>
#include <string>

#include "base.h"

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

  UniqueNode simplify(UniqueNode&& self) override;

  UniqueNode expand_add(UniqueNode&& self) override;

  uint64_t hash_code() const override;

  UniqueNode collect(UniqueNode&& self, uint64_t& hash) override;

  UniqueNode deep_copy() const override;
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERATOR_ADD_H
