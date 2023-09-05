#pragma once

#include <iostream>

template<long long x, long long y>
struct gcd {
    static constexpr const long long value = gcd<y, x% y>::value;
};

// specialize gcd
template<long long x>
struct gcd<x, 0> {
    static constexpr const long long value = x; 
};

// GCD for Rat Number
template<bool bNegN, bool bNegD, long long N, long long D>
struct rat_gcd;

// specialize for rat_gcd,Not Negative,Not Negative
template<long long N, long long D>
struct rat_gcd<false, false, N, D> { 
    static constexpr const long long value = gcd<N, D>::value; 
};

// speciazlize for rat_gcd,Negative,Negative
template<long long N, long long D>
struct  rat_gcd<true, true, N, D> { 
    static constexpr const long long value = gcd<-N, -D>::value; 
};

// specialize for rat_gcd,Not Negative,Negative
template<long long N, long long D>
struct rat_gcd<false, true, N, D> { 
    static constexpr const long long value = gcd<N, -D>::value; 
};

// speciazlize for rat_gcd,Negative,Not Negative
template<long long N, long long D>
struct  rat_gcd<true, false, N, D> { 
    static constexpr const long long value = gcd<-N, D>::value; 
};

// reduce
template<bool bZero, long long N, long long D>
struct  rat_reduce
{
    static const long long numer = 0;
    static const long long denom = 1;
};

template<long long N, long long D>
struct rat_reduce<false, N, D>
{
    static const long long _rat_gcd = rat_gcd<(N < 0), (D < 0), N, D>::value;

    static const long long numer = N / _rat_gcd;
    static const long long denom = D / _rat_gcd;
};

template<long long N, long long D = 1>
struct TNumberBasic
{
    using type = TNumberBasic<N, D>;
    static const long long numer = rat_reduce<(N == 0 || D == 0), N, D>::numer;
    static const long long denom = rat_reduce<(N == 0 || D == 0), N, D>::denom;
};

template <typename T>
struct display_imp_old
{
    static std::ostream& display(std::ostream& os, bool showBracket)
    {
        if (T::type::denom == 1)
        {
            return os << T::type::numer;
        }
        else
        {
            return os << static_cast<double>(T::type::numer) / \
                         static_cast<double>(T::type::denom);
        }
    }
};

template<typename T>
void displayOld(std::ostream& os = std::cout)
{
    display_imp_old<T>::display(os, true) << std::endl;
}
