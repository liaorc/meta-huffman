#include "huffman.h"

int main() {
    using HFM_Pair = TList<TList<MakeSymbol("AA"),TNumber<3>>,
                           TList<MakeSymbol("BB"),TNumber<2>>,
                           TList<MakeSymbol("CC"),TNumber<1>>,
                           TList<MakeSymbol("DD"),TNumber<1>>>;
    using HFM_Tree = GenerateHFMTree<HFM_Pair>;
    using Symbols = TList<MakeSymbol("AA"), MakeSymbol("DD"), MakeSymbol("AA"),MakeSymbol("BB"), 
                          MakeSymbol("BB"), MakeSymbol("CC"), MakeSymbol("AA")>;
    using Message = Encode<Symbols,HFM_Tree>;
    using Decoded = DecodeData<Message, HFM_Tree>;

    display<HFM_Tree>();
    display<Message>();
    display<Decoded>();
    return 0;
}
