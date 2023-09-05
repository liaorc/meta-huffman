#include "basic_impl.h"

int main() {
    length <TList<TNumberList<-1,10,100>, MakeSymbol("MetaC++"), TNumber<-5, 7>>> l1;
    length <TNumberList<-1, 2, 32, 222, -900>> l2;
    std::cout << l1.numer << std::endl; 
    std::cout << l2.numer << std::endl;

    list_ref <TList<TNumberList<-1, 10, 100>, MakeSymbol("MetaC++"), TNumber<-5, 7>>, TNumber<2>> lr;
        std::cout << is_number<decltype(lr)>::value <<" value N:" << decltype(lr)::type::numer <<\
        " value D:" << decltype(lr)::type::denom << std::endl;
    return 0;
}
