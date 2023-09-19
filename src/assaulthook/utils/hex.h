#pragma once 
#include <iostream>
#include <iomanip>

struct HexCharStruct {
    unsigned char c;
    HexCharStruct(unsigned char _c) : c(_c) {}
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs) {
    return (o << std::hex << (int)hs.c);
}

inline HexCharStruct hex(unsigned char _c) {
    return HexCharStruct(_c);
}