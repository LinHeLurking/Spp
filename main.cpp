#include <iostream>
#include <map>

#include "Symbolic/Number.h"
#include "Symbolic/Compute.h"


using namespace std;
using namespace Spp::Numeric;
using namespace Spp::Compute;

int main() {
    Expr a{1};
    Expr b{2};
    Expr c{3};
    Expr d{4};
    Expr e{5};
    Expr f{"x"};
    Expr g{"y"};
    Expr h{"z"};
    auto x = pow((a / b), (c - d) * e) * pow(f - g, h);
    cout << x.to_string() << endl;
    cout << x.eval().to_string() << endl;
    return 0;
}
