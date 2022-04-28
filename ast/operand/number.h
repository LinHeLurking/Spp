#ifndef SPP_AST_OPPERAND_NUMBER_H
#define SPP_AST_OPPERAND_NUMBER_H

#include <cassert>
#include <memory>
#include <sstream>
#include <type_traits>
#include <utility>

#include "../node.h"
#include "base.h"
#include "smart_num/smart_num.h"

namespace Spp::__Ast {

class Number : public OperandBase {
 public:
  explicit Number(const __SmartNum::SmartNum& v) : value_(v) {}

  template <typename... T>
  requires std::is_constructible_v<__SmartNum::SmartNum, T...>
  explicit Number(T... v) : value_((v, ...)) {}

  std::string to_string() const override {
    std::stringstream ss;
    ss << value_;
    return ss.str();
  }

  bool is_number() const override { return true; }

  UniqueNode eval(UniqueNode&& self) override { return std::move(self); }

  UniqueNode deep_copy() const override {
    return UniqueNode(new Number(value_));
  }

  friend class NumberAccessor;

 private:
  using SmartNum = Spp::__SmartNum::SmartNum;
  SmartNum value_;
};

class NumberAccessor {
 public:
  static inline __SmartNum::SmartNum get_num_unchecked(const UniqueNode& node) {
    return static_cast<Number*>(node.get())->value_;
  }
};
}  // namespace Spp::__Ast

#endif  // !SPP_AST_OPPERAND_NUMBER_H
