#include <iostream>
#include <map>

#include "Symbolic/Number.h"
#include "Symbolic/Expression.h"


using namespace std;
using namespace Spp::Numeric;
using namespace Spp;

int main() {
    Expr a{1};
    Expr b{2};
    Expr c{3};
    Expr d{4};
    auto x = (a + b) * (c - d);
    cout << to_string(x) << endl;
    return 0;
}
