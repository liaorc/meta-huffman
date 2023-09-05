#pragma once

#include <stdint.h>
#include <string>
#include <string_view>
#include <iostream>
#include <optional>

template<typename T,std::size_t N>
struct FixStringNew_
{
    using charType = T;
    constexpr static std::size_t length = N;
    charType data[length + 1] = {};

    // 用enbale_if编译期条件语句判断_M与0要相等，进行检查，通过才能= 0，可以更安全
    // 如果不能通过，那么= 0就会失败，也就会SFINAE，会报对应的构造找不到
    template<std::size_t _M = N, std::enable_if_t<_M == 0, std::size_t> = 0>
    constexpr FixStringNew_() :data{ 0 }
    {
    }

    template<std::size_t _M = N, std::enable_if_t<_M == 1, std::size_t> = 0>
    constexpr FixStringNew_(charType c) : data{ c,0 }
    {
    }

    // 利用类型推导，用basic_string_view<charType>的类型进行一次检查，可以更安全
    constexpr FixStringNew_(std::basic_string_view<charType> view)
    {
        for(std::size_t i = 0; i < length;i++)
        {
            data[i] = view[i];
        }
        data[N] = 0;
    }

    constexpr FixStringNew_(const charType (&pStr)[N+1])
    {
        for (std::size_t i = 0; i < length; i++)
        {
            data[i] = pStr[i];
        }
        data[N] = 0;
    }

    template<typename ...C>
    constexpr FixStringNew_(std::in_place_t, C...c) : data{static_cast<charType>(c)...,0 }
    {
    }
};

// 对参数如果是字符串数组进行重定返回
template<typename T,std::size_t N>
FixStringNew_(const T(&)[N])->FixStringNew_<T, N - 1>;



template<FixStringNew_ Str>
struct FixStringNew
{
    // 提取出模板参数的类型
    using charType = typename decltype(Str)::charType;
    static constexpr charType* Data() { return Str.data; }
    static constexpr std::size_t Length() { return Str.length; }
    static constexpr std::basic_string_view<charType> View() { return Str.data; }
    constexpr operator std::basic_string_view<charType>() { return View(); }

    constexpr static void InfoPrint()
    {
        std::cout << "Type:FixStringNew" << "   Length:" << \
            Str.length << "   String:" << Str.data << std::endl;
    }
};


#define MAKESTRINGNEW(str) \
([]{constexpr std::basic_string_view s{str};\
    return FixStringNew<FixStringNew_<typename decltype(s)::value_type, s.size()>{str}>{};}\
())


template<char ...C>
using FixString_S = FixStringNew < FixStringNew_<char, sizeof...(C)>{std::in_place,C...} > ;
