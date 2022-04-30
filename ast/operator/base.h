#ifndef SPP_AST_OPERATOR_BASE_H
#define SPP_AST_OPERATOR_BASE_H

#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "../node.h"
#include "../operand/number.h"
// #include "mul.h"

namespace Spp::__Ast {

enum class PosType { prefix_op, prefix_func, infix };

class OperatorBase : public Node {
 protected:
  friend class MulOp;
  using SmartNum = __SmartNum::SmartNum;
  std::string name_;
  uint priority_;
  PosType pos_;

  inline void eval_sub_tree() {
    for (uint32_t i = 0; i < child_.size(); ++i) {
      child_[i] = std::move(child_[i]->eval(std::move(child_[i])));
    }
  }

  inline void expand_add_sub_tree() {
    for (uint32_t i = 0; i < child_.size(); ++i) {
      child_[i] = std::move(child_[i]->expand_add(std::move(child_[i])));
    }
  }

  inline bool all_child_num() const {
    for (const auto &child : child_) {
      if (child->tag() != NodeTag::Number) {
        return false;
      }
    }
    return true;
  }

  template <uint64_t N>
  inline std::array<SmartNum, N> get_num_unchecked() const {
    assert(N == child_.size());
    std::array<SmartNum, N> ans;
    for (uint64_t i = 0; i < N; ++i) {
      ans[i] = NumberAccessor::get_num_unchecked(child_[i]);
    }
    return ans;
  }

  inline std::vector<SmartNum> get_num_unchecked() const {
    // assert(N == child_.size());
    std::vector<SmartNum> ans;
    for (uint64_t i = 0; i < child_.size(); ++i) {
      ans.emplace_back(NumberAccessor::get_num_unchecked(child_[i]));
    }
    return ans;
  }

  inline uint64_t combine_child_hash() const {
    uint64_t ans = 0;
    for (uint32_t i = 0; i < child_.size(); ++i) {
      ans = ans ^ (child_[i]->hash_code() << 1);
    }
    return ans;
  }

 public:
  std::vector<UniqueNode> child_;

  template <typename StrT, typename... NodeT>
  requires std::is_constructible_v<std::string, StrT> &&
      (std::is_same_v<UniqueNode, std::decay_t<NodeT>> &&...)
          OperatorBase(StrT name, uint priority, PosType pos, NodeT &&...args)
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
  OperatorBase(StrT name, uint priority, PosType pos, RandIt begin, RandIt end)
      : name_(name), priority_(priority), pos_(pos) {
    for (auto it = begin; it != end; ++it) {
      child_.emplace_back(std::move(*it));
    }
  }

  const auto &name() const { return name_; }
  uint priority() const override { return priority_; }

  NodeTag tag() const override { return NodeTag::Operator; }

  UniqueNode expand_add(UniqueNode &&self) override {
    expand_add_sub_tree();
    return std::move(self);
  }

  std::string to_string() const override {
    std::stringstream ss;
    switch (pos_) {
      case PosType::prefix_op: {
        ss << name_;
        for (auto &child : child_) {
          ss << child->to_string();
        }
        break;
      }
      case PosType::prefix_func: {
        ss << name_ << '(';
        for (uint i = 0; i < child_.size(); ++i) {
          ss << child_[i]->to_string();
          if (i != child_.size() - 1) ss << ", ";
        }
        ss << ')';
        break;
      }
      case PosType::infix: {
        if (child_[0]->priority() < priority_) ss << '(';
        ss << child_[0]->to_string();
        if (child_[0]->priority() < priority_) ss << ')';
        if (child_.size() > 1) ss << ' ' << name_ << ' ';
        // Under most cases, there is only one child after 0.
        // But things are different for multi-add-op.
        for (uint32_t i = 1; i < child_.size(); ++i) {
          if (child_[i]->priority() < priority_) ss << '(';
          ss << child_[i]->to_string();
          if (child_[i]->priority() < priority_) ss << ')';
          if (i != child_.size() - 1) {
            ss << ' ' << name_ << ' ';
          }
        }
        break;
      }
    }
    return ss.str();
  }

// Test helpers
#ifndef NDEBUEG
  friend class AstTestHelper;
#endif
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERATOR_BASE_H
