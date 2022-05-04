#include "add.h"

#include <algorithm>
#include <unordered_map>

#include "../operand/number.h"
#include "base.h"
#include "mul.h"
#include "neg.h"

namespace Spp::__Ast {

UniqueNode AddOp::simplify(UniqueNode &&self) {
  assert(self.get() == this);
  simplify_sub_tree();
  if (all_child_num()) {
    // auto [l, r] = get_num_unchecked<2>();
    auto child_num = get_child_num_unchecked();
    SmartNum val{0};
    for (auto &num : child_num) {
      val = val + num;
    }
    return UniqueNode(new Number(val));
  }
  return std::move(self);
}

UniqueNode AddOp::expand_add(UniqueNode &&self) {
  assert(this == self.get());
  expand_add_sub_tree();
  std::vector<UniqueNode> alt;
  alt.reserve(child_.size());
  for (auto it = child_.rbegin(); it != child_.rend(); ++it) {
    alt.emplace_back(std::move(*it));
  }
  child_.clear();
  for (auto it = alt.rbegin(); it != alt.rend(); ++it) {
    if ((*it)->tag() == NodeTag::Operator) {
      auto x = static_cast<OperatorBase *>(it->get());
      if (x->name() == "+") {
        for (auto jt = x->child_.rbegin(); jt != x->child_.rend(); ++jt) {
          child_.emplace_back(std::move(*jt));
        }
      }
      // After child expand_add, (a-b) will be expanded into (a+(-b)).
      // So, there is no need to specially consider SubOp.
      else {
        child_.emplace_back(std::move(*it));
      }
    } else {
      child_.emplace_back(std::move(*it));
    }
  }
  return std::move(self);
}

UniqueNode AddOp::collect(UniqueNode &&self, uint64_t &hash) {
  assert(this == self.get());
  std::unordered_map<uint64_t, std::vector<int>> hash_to_pos;
  uint64_t hs;
  std::vector<UniqueNode> alt;
  uint64_t tmp = 0;
  for (int i = 0; i < child_.size(); ++i) {
    alt.emplace_back(std::move(child_[i]->collect(std::move(child_[i]), hs)));
    tmp = tmp ^ (hs << 1);
    if (hash_to_pos.find(hs) == hash_to_pos.end()) {
      hash_to_pos[hs] = std::vector<int>(1, i);
    } else {
      hash_to_pos[hs].push_back(i);
    }
  }
  child_.clear();
  for (auto &[k, v] : hash_to_pos) {
    if (v.size() == 1) {
      child_.emplace_back(std::move(alt[v[0]]));
    } else {
      UniqueNode l = std::move(alt[v[0]]);
      UniqueNode r = UniqueNode(new Number((int)v.size()));
      child_.emplace_back(new MulOp(std::move(l), std::move(r)));
    }
  }
  hash = ADD_OP_HASH_SEED ^ (tmp << 1);
  return std::move(self);
}

UniqueNode AddOp::reorder(UniqueNode &&self, uint64_t &size) {
  assert(this == self.get());
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
                      // This leads a O(n^2) call over all sub-tree entries.
                      // Is there any chance to improve?
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

uint64_t AddOp::hash_code() const {
  return ADD_OP_HASH_SEED ^ (combine_child_hash() << 1);
}

UniqueNode AddOp::deep_copy() const {
  return UniqueNode(new AddOp(child_[0]->deep_copy(), child_[1]->deep_copy()));
}

}  // namespace Spp::__Ast
