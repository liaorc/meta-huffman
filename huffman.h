#pragma once
#include "basic_impl.h"

template<typename NodeSymbol,typename NodeWeight>
struct MakeLeaf:public TList<
    MakeSymbol("leaf"),NodeSymbol,NodeWeight>{};

template<typename object>
struct IsLeafNode:public is_equal<car<object>,MakeSymbol("leaf")>{};

// 获取字符节点,TList<"leaf",symbol,weight>
// 先右cdr， 再左car
template<typename node>
struct NodeSymbolLeaf:public TCadr<node>{};
    
// 获取权重节点
// 先右cdr,再右cdr,再左car
template<typename node>
struct NodeWeightLeaf:public TCaddr<node>{};
    
// 从node提取字符节点,如果不是叶子节点，就用TCaddr
// 注意这里有个TCaddr
template<typename node>
struct NodeSymbol:public if_else<
    IsLeafNode<node>,
        TList<NodeSymbolLeaf<node>>,
        TCaddr<node>
    >{};
    
// 从node权重节点，如果不是叶子节点，就用TCadddr
// 注意这里有个TCadddr
template<typename node>
struct NodeWeight:public if_else<
    IsLeafNode<node>,
        NodeWeightLeaf<node>,
        TCadddr<node>
    >{};

template<typename x,typename set>
struct AdjoinSet
{
    // 1.如果set为空返回TList<x>
    // 2.不然的话，如果x的权重小于set第一个元素的权重返回TCons连接x,set
    // 3.否则返回组合set的第一个元素，然后再将右边的set序列与x再递规
    // 总结下来，就是将x插到合适的位置才停止，不然一直交换
    using type = typename TCond<
        is_null<set>,
        TList<x>,
        is_less<NodeWeight<x>,NodeWeight<car<set>>>,
        TCons<x,set>,
        TCons<
            car<set>,
            AdjoinSet<x,cdr<set>>>
        >::type;
    using tag = typename type::tag;
};

// 生成叶子结点序列
template<typename pairs>
struct MakeLeafSet
{
    struct Else
    {
        // 取出第一个序列中元素
        using pair_ = car<pairs>;
        using type = typename AdjoinSet<
            MakeLeaf<
                car<pair_>,
                TCadr<pair_>>,
            MakeLeafSet<
                cdr<pairs>>
            >::type;
    };

    // 如果要传入序列为空，返回为空
    // 否则利用Else开始生成叶子节点,并调节序列
    using type = typename if_else<is_null<pairs>,
        TNull,Else>::type;
    using tag = typename type::tag;
};

template<typename lnode,typename rnode>
struct MakeNonLeaf
{
    using type = typename TList<
        lnode,rnode,
        append<
            NodeSymbol<lnode>,
            NodeSymbol<rnode>>,
        add<
            NodeWeight<lnode>,
            NodeWeight<rnode>>
        >::type;
    using tag = typename type::tag;
};

template<typename set>
struct SuccessivMerge
{
    struct Else
    {
        // set第一个元素
        using left = car<set>;
        // set右边第一个元素
        using right = TCadr<set>;
        // set右边剩下的元素
        using remained = TCddr<set>;
        // 调用SuccessivMerge类进行构造
        // 用AdjoinSet调整顺序
        // 用前两个元素，构造树节点
        // 用剩下的元素作为新序列
        using type = typename SuccessivMerge<
            AdjoinSet<
                MakeNonLeaf<left,right>,
                remained>
            >::type;
    };

    // 如果set长度为1，直接返回第一个元素
    // 否则调用Else进行构造
    using type = typename if_else<
        is_equal<
            length<set>,
            TNumber<1>>,
        car<set>,
        Else>::type;
    using tag = typename type::tag;
};

// 直接生成Huffman树，借用Leaf节点序列
template<typename pairs>
struct GenerateHFMTree:public SuccessivMerge<
    typename MakeLeafSet<pairs>::type>{};

// 左分支首节点
template<typename tree>
struct left_path:public car<tree>{};

// 右分支首节点
template<typename tree>
struct right_path:public TCadr<tree>{};

// 单字符转bit编码
template<typename symbol,typename tree>
struct EncodeSymbol
{
    struct Else
    {
        struct Else2
        {
            // 对tree的右分支进行编码
            using Right_Result = EncodeSymbol<symbol,right_path<tree>>;
            // 如果右分支合法，用TCons建立右分支与1的连接，作为返回，否
            // 则返否false
            using type = typename if_else<Right_Result,
                TCons<
                    TNumber<1>,
                    Right_Result>,
                TBoolean<false>>::type;
        };

        // 对tree的左分支进行编码
        using Left_Result = EncodeSymbol<symbol,left_path<tree>>;
        // 如果左分支合法，用TCons建立右分支与0的连接，作为返回，否
        // 则返否对右分支的Else2计算
        using type = typename if_else<Left_Result,
            TCons<
                TNumber<0>,
            Left_Result>,
            Else2>::type;
    };
    // 1,如果是叶子节点，判断是不是字符节点是否相等
    // 2,如果相等，返回TNull，否则会false，主要用于后面的递归判断
    // 3,否则进行Else进一步处理
    using type = typename if_else<
        IsLeafNode<tree>,
            if_else<
                is_equal<symbol,NodeSymbolLeaf<tree>>,
                    TNull,
                    TBoolean<false>>,
             Else>::type;
    using tag = typename type::tag;
};

// 对待发送的信息利用已经构建好的Huffman树进行编码
template<typename message,typename tree>
struct Encode
{
    using type = typename if_else<
        is_null<message>,
            TNull,
            append<
                EncodeSymbol<car<message>,tree>,
                Encode<cdr<message>,tree>>
        >::type;
    using tag = typename type::tag;
};

// 根据0，1选择分支
// 0对应左分支，1对应右分支
template<typename bit,typename path>
struct choose_path
{
    using type = typename TCond<
        is_equal<bit,TNumber<0>>,
            left_path<path>,
            is_equal<bit,TNumber<1>>,
                right_path<path>,
            TNull>::type;
    using tag = typename type::tag;
};

// 概据bit位，解码
template<typename bits,typename tree>
struct DecodeData
{
    template<typename bits_In,typename current_Path>
    struct Decode
    {
        struct Else
        {
            // 概据传入01110...的bits位信息，选择左右分支
            using next_path = choose_path<car<bits_In>,current_Path>;
            using type = typename if_else<IsLeafNode<next_path>,
                TCons<
                    NodeSymbolLeaf<next_path>,
                    Decode<
                        cdr<bits_In>,
                        tree>>,
                Decode<
                    cdr<bits_In>,
                    next_path>
                >::type;
        };

        using type = typename if_else<is_null<bits_In>,TNull,Else>::type;
    };
    using type = typename Decode<bits,tree>::type;
    using tag = typename type::tag;
};
