#ifndef SPP_AST_OPERATOR_BASE_H
#define SPP_AST_OPERATOR_BASE_H

#include <array>
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

#include "../node.h"
#include "../operand/number.h"
#include "../operand/variable.h"

namespace Spp::__Ast {

enum class PosType { prefix_op, prefix_func, infix };

class OperatorBase : public Node {
 public:
  std::vector<UniqueNode> child_;
  template <typename StrT, typename... NodeT>
  requires std::is_constructible_v<std::string, StrT> &&
      (std::is_same_v<UniqueNode, std::decay_t<NodeT>> &&...)OperatorBase(
          StrT name, uint32_t priority, PosType pos, NodeT &&...args)
      : name_(std::move(name)), priority_(priority), pos_(pos) {
    (child_.push_back(std::move(args)), ...);
  }

  template <typename StrT, typename RandIt>
  requires std::is_constructible_v<std::string, StrT> && requires {
    // Can increment
    std::declval<RandIt>()++;
    // Can campare
    std::declval<RandIt>() == std::declval<RandIt>();
    std::declval<RandIt>() != std::declval<RandIt>();
  } && std::is_same_v<UniqueNode,
                      std::decay_t<decltype(*std::declval<RandIt>())>>
  OperatorBase(StrT name, uint32_t priority, PosType pos, RandIt begin,
               RandIt end)
      : name_(name), priority_(priority), pos_(pos) {
    for (auto it = begin; it != end; ++it) {
      child_.emplace_back(std::move(*it));
    }
  }

  const std::string &name() const;

  uint32_t priority() const override;

  uint64_t size() const override;

  NodeTag tag() const override;

  UniqueNode expand_add(UniqueNode &&self) override;

  std::string to_string() const override;

 protected:
  friend class MulOp;
  using SmartNum = __SmartNum::SmartNum;
  std::string name_;
  uint32_t priority_;
  PosType pos_;

  void simplify_sub_tree();

  void expand_add_sub_tree();

  // void reorder_sub_tree();

  bool all_child_num() const;

  template <uint64_t N>
  std::array<SmartNum, N> get_child_num_unchecked() const {
    assert(N == child_.size());
    std::array<SmartNum, N> ans;
    for (int i = 0; i < N; ++i) {
      ans[i] = NumberAccessor::get_num_unchecked(child_[i]);
    }
    return ans;
  }

  std::vector<SmartNum> get_child_num_unchecked() const;

  static inline SmartNum get_num_unchecked(const UniqueNode &node) {
    return NumberAccessor::get_num_unchecked(node);
  }

  static inline const std::string &get_name_unchecked(const UniqueNode &node) {
    return VariableAccessor::get_name_unchecked(node);
  }

  uint64_t combine_child_hash() const;
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERATOR_BASE_H
