#include <iostream>

#include "Symbolic/Number.h"
#include "Symbolic/Expression.h"


using namespace std;
using namespace Spp::Numeric;
using namespace Spp;

int main() {
    auto x = Expr(1);
    auto y = Expr(2) / Expr(3);
    auto z = Expr(4);
    auto expr = (x + y) / z;
    cout << expr << endl;
    cout << expr.eval() << endl;
    cout << expr.as_type<Rational<int>>().eval() << endl;
    cout << expr.as_type<double>().eval() << endl;
    return 0;
}
