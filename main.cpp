#include <iostream>

#include "Symbolic/Number.h"
#include "Symbolic/Expression.h"


using namespace std;
using namespace Spp::Numeric;
using namespace Spp;

int main() {
    Expr x = Rational(1, 2);
    Expr y = 3.4;
    Expr z = 5.6;
    auto expr = (x + y) / z;
    cout << expr << endl;
    cout << expr.eval() << endl;
    return 0;
}
