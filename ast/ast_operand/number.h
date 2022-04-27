#ifndef AST_OPPERAND_NUMBER_H
#define AST_OPPERAND_NUMBER_H

#include <sstream>
#include <type_traits>

#include "../ast_node.h"
#include "../smart_num/smart_num.h"
#include "base.h"

namespace Spp::__Ast {
class Number : public OperandBase {
 public:
  template <typename T>
  requires std::is_constructible_v<T, Spp::__SmartNum::SmartNum>
  explicit Number(const T& v) : value_(v) {}

  std::string to_string() const override {
    std::stringstream ss;
    ss << value_;
    return ss.str();
  }

  UniqueNode deep_copy() const override {
    return UniqueNode(new Number(value_));
  }

 private:
  using SmartNum = Spp::__SmartNum::SmartNum;
  SmartNum value_;
};
}  // namespace Spp::__Ast

#endif  // !AST_OPPERAND_NUMBER_H
