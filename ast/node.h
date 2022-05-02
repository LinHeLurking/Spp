#ifndef SPP_AST_NODE_H
#define SPP_AST_NODE_H

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

namespace Spp::__Ast {

class Node;

using UniqueNode = std::unique_ptr<Node>;

template <typename T>
inline constexpr bool is_unique_node =
    std::is_same_v<UniqueNode, std::decay_t<T>>;

enum class NodeTag {
  Number,
  Variable,
  Operator,
};

class Node {
 public:
  virtual ~Node() {}
  virtual uint priority() const = 0;

  virtual std::string to_string() const = 0;

  /**
   * Indicate the node category, which is one of Number, Variable and Operator.
   * One category contains several "types" of Node.
   * Every operator except sub or neg, has unique operator name.
   * You can use operator name to check its type.
   */
  virtual NodeTag tag() const = 0;

  /**
   * Simplify as much as possible.
   */
  virtual UniqueNode simplify(UniqueNode&& self) = 0;

  /**
   * Expand all "Add" operations.
   * Example:
   *     1) (1+2)*(3+4) => 1*3 + 1*4 + 2*3 + 2*4
   *     2) 1-2 => 1 + (-2)
   */
  virtual UniqueNode expand_add(UniqueNode&& self) = 0;

  /**
   * Reorder commutative operations on demmand.
   */
  virtual UniqueNode reorder(UniqueNode&& self) {
    assert(this == self.get());
    return std::move(self);
  }

  /**
   * Collect similar terms, returning hash_code via `hash` parameter.
   */
  virtual UniqueNode collect(UniqueNode&& self, uint64_t& hash) {
    assert(this == self.get());
    hash = hash_code();
    return std::move(self);
  }

  virtual UniqueNode deep_copy() const = 0;

  virtual uint64_t hash_code() const = 0;

  friend inline std::ostream& operator<<(std::ostream& os, const Node& n) {
    os << n.to_string();
    return os;
  }
};

}  // namespace Spp::__Ast

#endif  // !AST_NODE_H
