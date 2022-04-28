#ifndef SPP_AST_OPERATOR_BASE_H
#define SPP_AST_OPERATOR_BASE_H

#include <cstdint>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "../node.h"

namespace Spp::__Ast {

enum class PosType { prefix_op, prefix_func, infix };

class OperatorBase : public Node {
 private:
  std::string name_;
  uint priority_;
  std::vector<UniqueNode> child_;
  PosType pos_;

 public:
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

  std::string to_string() const override {
    std::stringstream ss;
    switch (pos_) {
      case PosType::prefix_op: {
        ss << name_;
        for (auto &child : child_) {
          ss << child->to_string();
        }
      }
      case PosType::prefix_func: {
        ss << name_ << '(';
        for (uint i = 0; i < child_.size(); ++i) {
          ss << child_[i]->to_string();
          if (i != child_.size() - 1) ss << ", ";
        }
        ss << ')';
      }
      case PosType::infix: {
        if (child_[0]->priority() < priority_) ss << '(';
        ss << child_[0]->to_string();
        if (child_[0]->priority() < priority_) ss << ')';
        ss << ' ' << name_ << ' ';
        if (child_[1]->priority() < priority_) ss << '(';
        ss << child_[1]->to_string();
        if (child_[1]->priority() < priority_) ss << ')';
      }
    }
    return ss.str();
  }

  UniqueNode deep_copy() const override {
    std::vector<UniqueNode> copy_child;
    copy_child.reserve(child_.size());
    for (auto &child : child_) {
      copy_child.emplace_back(child->deep_copy());
    }
    auto content = new OperatorBase(name_, priority_, pos_, copy_child.begin(),
                                    copy_child.end());
    return UniqueNode(content);
  }

// Test helpers
#ifndef NDEBUEG
  friend class AstTestHelper;
#endif
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERATOR_BASE_H
