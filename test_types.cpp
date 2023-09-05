#include "types.h"

int main() {
    using mylst= TList<TNumber<1,2>, TNumber<2,1>, TBoolean<true>,TBoolean<false>, MakeSymbol("123")>;
    mylst lstObj;
    bool bPair = is_pair<decltype(lstObj)>::value;
    display<mylst>();
    return 0;
}
