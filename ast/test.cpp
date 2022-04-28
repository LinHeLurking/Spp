#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <vector>

#include "node.h"
#include "operand/number.h"
#include "operator/add.h"
#include "operator/base.h"
#include "operator/div.h"
#include "operator/mul.h"
#include "operator/sub.h"

using std::cout;
using std::endl;
using std::move;
using std::remove_if;
// using std::unique_ptr;
// using std::vector;

namespace Spp::__Ast {

class AstTestHelper {
 public:
  static void set_nth_child(OperatorBase* node, uint n, UniqueNode v) {
    node->child_[n] = move(v);
  }
};

#define COPY_CHANGE_TEST(op_name, op_func)                        \
  std::string x, y;                                               \
  auto a = UniqueNode(new Number(1));                             \
  auto b = UniqueNode(new Number(2));                             \
  cout << "a: " << *a << endl;                                    \
  cout << "b: " << *b << endl;                                    \
  auto a_copy = a->deep_copy();                                   \
  auto b_copy = b->deep_copy();                                   \
  cout << "a_copy: " << a_copy->to_string() << endl;              \
  cout << "b_copy: " << b_copy->to_string() << endl;              \
  auto c = op_func(a, b);                                         \
  auto c_copy = c.deep_copy();                                    \
  cout << "c: " << c.to_string() << endl;                         \
  cout << "c_copy: " << c_copy->to_string() << endl;              \
  x = c.to_string();                                              \
  y = c_copy->to_string();                                        \
  x.erase(remove_if(x.begin(), x.end(), isspace), x.end());       \
  y.erase(remove_if(y.begin(), y.end(), isspace), y.end());       \
  EXPECT_EQ(x, "1" op_name "2");                                  \
  EXPECT_EQ(y, "1" op_name "2");                                  \
  Node* base_p = c_copy.get();                                    \
  OperatorBase* op = dynamic_cast<OperatorBase*>(base_p);         \
  AstTestHelper::set_nth_child(op, 0, UniqueNode(new Number(3))); \
  AstTestHelper::set_nth_child(op, 1, UniqueNode(new Number(4))); \
  cout << "c: " << c.to_string() << endl;                         \
  cout << "c_copy: " << c_copy->to_string() << endl;              \
  x = c.to_string();                                              \
  y = c_copy->to_string();                                        \
  x.erase(remove_if(x.begin(), x.end(), isspace), x.end());       \
  y.erase(remove_if(y.begin(), y.end(), isspace), y.end());       \
  EXPECT_EQ(x, "1" op_name "2");                                  \
  EXPECT_EQ(y, "3" op_name "4");

TEST(AstTest, AddTest) { COPY_CHANGE_TEST("+", AddOp); }
TEST(AstTest, SubTest) { COPY_CHANGE_TEST("-", SubOp); }
TEST(AstTest, MulTest) { COPY_CHANGE_TEST("*", MulOp); }
TEST(AstTest, DivTest) { COPY_CHANGE_TEST("/", DivOp); }

}  // namespace Spp::__Ast
