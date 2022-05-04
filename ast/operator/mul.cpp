#include "mul.h"

#include "add.h"

namespace Spp::__Ast {

UniqueNode MulOp::simplify(UniqueNode &&self) {
  assert(self.get() == this);
  simplify_sub_tree();
  if (all_child_num()) {
    auto [l, r] = get_child_num_unchecked<2>();
    return UniqueNode(new Number(l * r));
  }
  return std::move(self);
}

UniqueNode MulOp::expand_add(UniqueNode &&self) {
  expand_add_sub_tree();
  std::vector<UniqueNode> l, r, child;
  auto take = [&](UniqueNode &from, std::vector<UniqueNode> &to) {
    if (from->tag() != NodeTag::Operator) {
      to.emplace_back(std::move(from));
    } else {
      auto tmp = static_cast<OperatorBase *>(from.get());
      for (auto &x : tmp->child_) {
        to.emplace_back(std::move(x));
      }
    }
  };
  take(child_[0], l);
  take(child_[1], r);
  for (auto &x : l) {
    for (auto &y : r) {
      // TODO: Actually some copy can be saved.
      child.emplace_back(new MulOp(x->deep_copy(), y->deep_copy()));
    }
  }
  std::move_iterator begin{child.begin()}, end{child.end()};
  return UniqueNode(new AddOp(begin, end));
}

UniqueNode MulOp::reorder(UniqueNode &&self, uint64_t &size) {
  assert(this == self.get());
  // In the future, there might be some matrix operands.
  // So, pay attention to MulOp reorder!
  size = 1;
  using T = std::pair<UniqueNode, uint64_t>;
  std::vector<T> sized_child;
  for (int i = 0; i < child_.size(); ++i) {
    uint64_t sz;
    UniqueNode c = child_[i]->reorder(std::move(child_[i]), sz);
    size += sz;
    sized_child.emplace_back(std::move(c), sz);
  }
  child_.clear();
  std::sort(sized_child.begin(), sized_child.end(),
            [](const T &l, const T &r) -> bool {
              switch (l.first->tag()) {
                case NodeTag::Number: {
                  if (r.first->tag() == NodeTag::Number) {
                    return get_num_unchecked(l.first) <
                           get_num_unchecked(r.first);
                  } else {
                    return true;
                  }
                }
                case NodeTag::Variable: {
                  switch (r.first->tag()) {
                    case NodeTag::Number: {
                      return false;
                    }
                    case NodeTag::Variable: {
                      return get_name_unchecked(l.first) <
                             get_name_unchecked(r.first);
                    }
                    case NodeTag::Operator: {
                      return true;
                    }
                  }
                }
                case NodeTag::Operator: {
                  if (r.first->tag() == NodeTag::Operator) {
                    if (l.second == r.second) {
                      return l.first->to_string() < r.first->to_string();
                    } else {
                      return l.second < r.second;
                    }
                  } else {
                    return false;
                  }
                }
              }
            });
  for (int i = 0; i < sized_child.size(); ++i) {
    child_.emplace_back(std::move(sized_child[i].first));
  }
  return std::move(self);
}

uint64_t MulOp::hash_code() const {
  return MUL_OP_HASH_CODE ^ (combine_child_hash() << 1);
}

UniqueNode MulOp::deep_copy() const {
  return UniqueNode(new MulOp(child_[0]->deep_copy(), child_[1]->deep_copy()));
}

}  // namespace Spp::__Ast
