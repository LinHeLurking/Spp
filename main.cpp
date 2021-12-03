#include <iostream>
#include <vector>

#include "Symbolic/Number.h"
#include "Symbolic/Expression.h"


using namespace std;
using namespace Spp::Numeric;
using namespace Spp::Expression;

int main() {
    auto l = NumericNode(Rational(1, 5));
    auto r = NumericNode(Rational(1, 5));
    auto rt_add = CastNode<double, Rational<int>>(l + r);
    cout << rt_add.val() << endl;
    return 0;
}
