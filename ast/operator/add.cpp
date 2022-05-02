#include "add.h"

#include "mul.h"

namespace Spp::__Ast {

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

}  // namespace Spp::__Ast
