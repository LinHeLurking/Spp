#ifndef SPP_AST_OPERAND_VARIABLE_H
#define SPP_AST_OPERAND_VARIABLE_H

#include <functional>
#include <string>
#include <type_traits>

#include "../node.h"
#include "base.h"

namespace Spp::__Ast {
class Variable : public OperandBase {
 public:
  template <typename... T>
  requires std::is_constructible_v<std::string, T...>
  explicit Variable(T &&...name) : name_((name, ...)) {}

  std::string to_string() const override { return name_; }

  NodeTag tag() const override { return NodeTag::Variable; }

  UniqueNode eval(UniqueNode &&self) override {
    assert(self.get() == this);
    return std::move(self);
  }

  UniqueNode deep_copy() const override {
    return UniqueNode(new Variable(name_));
  }

  uint64_t hash_code() const override {
    return std::hash<std::string>{}(name_);
  }

 private:
  std::string name_;
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERAND_VARIABLE_H
