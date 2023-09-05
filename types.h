#pragma once

#include "fix_string.h"
#include "number.h"

struct number_tag
{
    static const bool is_pair = false;
    static const bool is_null = false;
    static const bool is_number = true;
    static const bool is_boolean = false;
    static const bool is_symbol = false;
};

struct null_tag
{
    static const bool is_pair = false;
    static const bool is_null = true;
    static const bool is_number = false;
    static const bool is_boolean = false;
    static const bool is_symbol = false;
};

struct pair_tag
{
    static const bool is_pair = true;
    static const bool is_null = false;
    static const bool is_number = false;
    static const bool is_boolean = false;
    static const bool is_symbol = false;
};

struct boolean_tag
{
    static const bool is_pair = false;
    static const bool is_null = false;
    static const bool is_number = false;
    static const bool is_boolean = true;
    static const bool is_symbol = false;
};

struct symbol_tag
{
    static const bool is_pair = false;
    static const bool is_null = false;
    static const bool is_number = false;
    static const bool is_boolean = false;
    static const bool is_symbol = true;
};

template<bool bFlag>
struct TBoolean
{
    using tag = boolean_tag;
    using type = TBoolean<bFlag>;
    static const bool value = bFlag;
};

struct TNull
{
    using tag = null_tag;
    using type = TNull;
};

template<typename T, typename U>
struct TPair
{
    using tag = pair_tag;
    using type = TPair<T, U>;
    using car_ = typename T::type;
    using cdr_ = typename U::type;
};

template<char...C>
struct TSymbol
{
    using type = TSymbol<C...>;
    using tag = symbol_tag;
    constexpr static char const value[sizeof...(C) + 1] = { C...,'\0' };
};

template<char ...C>
auto MakeSymbolOld(TSymbol<C...>)->TSymbol<C...> ;

// #define MakeSymbol(str) \
//     decltype(MakeSymbolOld(TSymbol<MakeCharIndexSequence(str)>{}))

template<char ...C>
auto AutoSplitString2(TSymbol<C...>)->
      decltype(MakeSymbolOld(TSymbol<C>{}...));

template<char ...C1,char C,char ...C2>
auto MakeSymbolOld(TSymbol<C1...>,TSymbol<C>,TSymbol<C2>...)->
    decltype(MakeSymbolOld(TSymbol<C1...,C>{},TSymbol<C2>{}...));

template<char ...C1,char ...C2>
auto MakeSymbolOld(TSymbol<C1...>,TSymbol<'\0'>,TSymbol<C2>...)->
       TSymbol<C1...>;

#define MakeSymbol(str) decltype(AutoSplitString2(TSymbol<MakeCharIndexSequence(str)>{}))

template<long long N, long long D = 1>
struct TNumber
{
    using type = TNumber<N, D>;
    using tag = number_tag;
    static const long long numer = rat_reduce<(N == 0 || D == 0), N, D>::numer;
    static const long long denom = rat_reduce<(N == 0 || D == 0), N, D>::denom;
};

template<typename T>
using is_null = TBoolean<T::tag::is_null>;

template<typename T>
using is_pair = TBoolean<T::tag::is_pair>;

template<typename T>
using is_number = TBoolean<T::tag::is_number>;

template<typename T>
using is_boolean = TBoolean<T::tag::is_boolean>;

template<typename T>
using is_symbol = TBoolean<T::tag::is_symbol>;


// containers
template<typename...T>
struct TList;

template<typename T>
struct TList<T> :public TPair<T, TNull> {};

template<typename T, typename ...X>
struct TList<T, X...> : public TPair<T, TList<X...>> {};

// List
template<int ...N>
struct TNumberList;

template<int N>
struct TNumberList<N> :public TPair<TNumber<N>, TNull> {};

template<int N, int ...M>
struct TNumberList<N, M...> :public TPair<TNumber<N>, TNumberList<M...>> {};

// car cdr cons
template<typename x>
struct car
{
    using type = typename x::type::car_;
    using tag = typename type::tag;
};

template<typename x>
struct cdr
{
    using type = typename x::type::cdr_;
    using tag = typename type::tag;
};

template<typename T, typename U>
struct TCons : public TPair<T, U> {};

template<typename x>
using TCadr = car<cdr<x>>;

template<typename x>
using TCddr = cdr<cdr<x>>;

template<typename x>
using TCaddr = car<cdr<cdr<x>>>;

template<typename x>
using TCadddr = car<cdr<cdr<cdr<x>>>>;

// print helper
template <typename T>
struct display_impl
{
    static std::ostream& display(std::ostream& os, bool showBracket, number_tag)
    {
        if (T::type::denom == 1)
        {
            return os << T::type::numer;
        }
        else
        {
            return os << static_cast<double>(T::type::numer) / static_cast<double>(T::type::denom);
        }
    }

    static std::ostream& display(std::ostream& os, bool showBracket, symbol_tag)
    {
        return os << T::type::value;
    }

    static std::ostream& display(std::ostream& os, bool showBracket, boolean_tag)
    {
        if (T::type::value)
        {
            os << "#t";
        }
        else
        {
            os << "#f";
        }
        return os;
    }

    static std::ostream& display(std::ostream& os, bool showBracket, null_tag)
    {
        os << "()";
        return os;
    }

    static std::ostream& display(std::ostream& os, bool showBracket, pair_tag)
    {
        if (showBracket)
        {
            os << "(";
        }
        display_impl<car<T>>::display(os, true);
        using cdrT = cdr<T>;
        if (is_null<cdrT>::value)
        {
            // nothing
        }
        else if (is_pair<cdrT>::value)
        {
            os << " ";
            display_impl<cdr<T>>::display(os, false);
        }
        else
        {
            os << " . ";
            display_impl<cdr<T>>::display(os, false);
        }
        if (showBracket)
        {
            os << ")";
        }
        return os;
    }

    static std::ostream& display(std::ostream& os, bool showBracket)
    {
        return display(os, showBracket, typename T::tag());
    }
};

template<typename T>
void display(std::ostream& os = std::cout)
{
    display_impl<T>::display(os, true) << std::endl;
}
