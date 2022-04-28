#ifndef SPP_AST_NODE_H
#define SPP_AST_NODE_H

#include <iostream>
#include <memory>
#include <string>

namespace Spp::__Ast {

class Node;

using UniqueNode = std::unique_ptr<Node>;

template <typename T>
inline constexpr bool is_unique_node =
    std::is_same_v<UniqueNode, std::decay_t<T>>;

class Node {
 public:
  virtual ~Node() {}
  virtual uint priority() const = 0;

  virtual std::string to_string() const = 0;

  virtual bool is_number() const { return false; }

  virtual bool is_variable() const { return false; }

  virtual UniqueNode eval(UniqueNode&& self) = 0;

  virtual UniqueNode deep_copy() const = 0;

  friend inline std::ostream& operator<<(std::ostream& os, const Node& n) {
    os << n.to_string();
    return os;
  }
};

}  // namespace Spp::__Ast

#endif  // !AST_NODE_H
