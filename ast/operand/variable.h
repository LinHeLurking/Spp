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
  template <typename T>
  requires std::is_constructible_v<std::string, T>
  explicit Variable(T &&name) : name_(name) {}

  std::string to_string() const override;

  NodeTag tag() const override;

  UniqueNode simplify(UniqueNode &&self) override;

  UniqueNode deep_copy() const override;

  uint64_t hash_code() const override;

  friend class VariableAccessor;

 private:
  std::string name_;
};

class VariableAccessor {
 public:
  static inline const std::string &get_name_unchecked(const UniqueNode &node) {
    return static_cast<Variable *>(node.get())->name_;
  }
};

}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPERAND_VARIABLE_H
