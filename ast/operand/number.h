#ifndef SPP_AST_OPPERAND_NUMBER_H
#define SPP_AST_OPPERAND_NUMBER_H

#include <sstream>
#include <type_traits>
#include <utility>

#include "../node.h"
#include "../smart_num/smart_num.h"
#include "base.h"

namespace Spp::__Ast {

using SmartNum = Spp::__SmartNum::SmartNum;

class Number : public OperandBase {
 public:
  explicit Number(const __SmartNum::SmartNum& v) : value_(v) {}

  template <typename... T>
  requires std::is_constructible_v<SmartNum, T...>
  explicit Number(T... v) : value_((v, ...)) {}

  std::string to_string() const override {
    std::stringstream ss;
    ss << value_;
    return ss.str();
  }

  UniqueNode deep_copy() const override {
    return UniqueNode(new Number(value_));
  }

 private:
  SmartNum value_;
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPPERAND_NUMBER_H
