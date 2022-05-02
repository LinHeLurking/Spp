#ifndef SPP_AST_H
#define SPP_AST_H

#include <cstdint>
#include <type_traits>

#include "../util/concept.h"
#include "node.h"
#include "operand/number.h"
#include "operand/variable.h"
#include "operator/add.h"
#include "operator/div.h"
#include "operator/mul.h"
#include "operator/neg.h"
#include "operator/sub.h"

namespace Spp::__Ast {
using Spp::__Concept::SignedInteger;
using Spp::__Concept::UnsignedInteger;
// using SmartNum = __SmartNum::SmartNum;
// Helpers to create ast UniqueNode
class UniqueNodes {
 public:
  // Create a number
  template <typename T>
  requires SignedInteger<T> || std::is_floating_point_v<T>
  static inline UniqueNode number(T x) { return UniqueNode(new Number(x)); }

  // Create a rational number
  template <typename T, typename U>
  requires UnsignedInteger<T> && UnsignedInteger<U>
  static inline UniqueNode number(T nominator, U denominator, uint32_t sign) {
    return UniqueNode(new Number(nominator, denominator, sign));
  }

  // Create a variable
  template <typename T>
  requires std::is_constructible_v<std::string, T>
  static inline UniqueNode variable(T&& name) {
    return UniqueNode(new Variable(name));
  }
};
}  // namespace Spp::__Ast

#endif  // !AST_H
