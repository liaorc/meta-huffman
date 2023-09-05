#pragma once
#include "types.h"

// if else
template<typename Flag, typename True, typename False>
struct if_else_impl :public True {};

template<typename True, typename False>
struct if_else_impl<TBoolean<false>, True, False> :public False {};

template<typename Flag, typename True, typename False>
struct if_else : public if_else_impl<typename Flag::type, True, False> {};

// condition
template<typename ...X>
struct TCond;

template<typename Flag, typename T, typename ...X>
struct TCond<Flag, T, X...> : public if_else<Flag, T, TCond<X...>> {};

template<typename Else>
struct TCond<Else> :public Else {};

// boolean operation
template<typename a, typename b>
struct or_ :public TBoolean<(a::type::value || b::type::value)> {};

template<typename a, typename b>
struct and_ :public TBoolean<(a::type::value && b::type::value)> {};

template<typename a>
struct not_ :public TBoolean<(!a::type::value)> {};

// basic operation
template<long long N0, long long D0, long long N1, long long D1>
using add_ = TNumber<N0* D1 + N1 * D0, D0* D1>; 

template<long long N0, long long D0, long long N1, long long D1>
using sub_ = TNumber<N0* D1 - N1 * D0, D0* D1>;

template<typename a, typename b>
struct add : public add_<a::type::numer, a::type::denom, b::type::numer, b::type::denom> {};

template<typename a, typename b>
struct sub : public sub_<a::type::numer, a::type::denom, b::type::numer, b::type::denom> {};

template<typename a, typename b>
struct mul : public TNumber<a::type::numer* b::type::numer, a::type::denom* b::type::denom> {};

template<typename a, typename b>
struct div_ : public TNumber<a::type::numer* b::type::denom, a::type::denom* b::type::numer> {};

// comparison
template<long long N0, long long D0, long long N1, long long D1>
using is_great_ = TBoolean<(N0* D1 > N1* D0)>;

template<long long N0, long long D0, long long N1, long long D1>
using is_less_ = TBoolean<(N0* D1 < N1* D0)>;

template<typename a, typename b>
struct is_greater :public is_great_<a::type::numer, a::type::denom, b::type::numer, b::type::denom> {};

template<typename a, typename b>
struct is_less :public is_less_<a::type::numer, a::type::denom, b::type::numer, b::type::denom> {};

template<typename a>
struct abs_ :public if_else<is_greater<a, TNumber<0>>, a, TNumber<-a::type::numer, a::type::denom>> {};

// work around for symbol pointers comparision
template<typename a, typename b, class = void>
struct is_equal_symbol_ : public TBoolean<false> {};

// when comparing different pointers, it is not a constexpr in c++ but a valid constexpr in clang
// but it is always true constexpr in both compiler
template<typename a, typename b>
struct is_equal_symbol_<a, b, std::enable_if_t<a::type::value == b::type::value>> : public TBoolean<true> {};

template<typename a, typename b, typename tagA, typename tagB>
struct is_equal_ :public TBoolean<false> {};

template<typename a, typename b>
struct is_equal_<a, b, number_tag, number_tag> :public TBoolean<a::type::numer == b::type::numer && a::type::denom == b::type::denom> {};

template<typename a, typename b>
struct is_equal_<a, b, symbol_tag, symbol_tag> :public is_equal_symbol_<a, b> {};

// problem occur when two pointers are compared
// template<typename a, typename b>
// struct is_equal_<a, b, symbol_tag, symbol_tag> :public TBoolean<a::type::value == b::type::value> {};

template<typename a, typename b>
struct is_equal :public is_equal_<a, b, typename a::tag, typename b::tag> {};

template<typename a, typename b>
struct not_equal :public not_<is_equal<a, b>> {};

// helper functions
// length of list
template<typename item>
struct length :public if_else<is_null<item>, TNumber<0>, add<TNumber<1>,length<cdr<item>>>>{};

// get kth element
template<typename item,typename n>
struct list_ref:public if_else<
           is_equal<n,TNumber<0>>,
              car<item>,
              list_ref<cdr<item>,sub<n,TNumber<1>>>>{};

// append to list
template<typename a,typename b>
struct append:public if_else<
         is_null<a>,
          b,
          TCons<car<a>,append<cdr<a>,b>>>{};
