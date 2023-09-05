#include "fix_string_new.h"

int main() {
    auto a12 = MAKESTRINGNEW("1234567890");
    
    a12.InfoPrint();
    FixString_S<'h', 'e','l','l','o'> a13;
    a13.InfoPrint();
    constexpr const char acTemp[6] = "12345";
    constexpr const char* p = "myhello";
    // constexpr auto a14 = MAKESTRINGNEW(acTemp);
    //FixStringNew<FixStringNew_<decltype(acTemp), 6>{acTemp}> a14;
    //a14.InfoPrint();
    FixStringNew<FixStringNew_("12345")> a14;
    a14.InfoPrint();
    return 0;
}
