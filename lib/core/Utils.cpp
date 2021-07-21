//
// Created by Guy on 14/07/2021.
//

#include <core/Utils.h>

bool Utils::Bits::GetBit(uint64_t field, int bitIdx) {
    if(bitIdx >= 64)
        return false;
    return ((field >> bitIdx) & 1) == 1;
}

uint64_t Utils::Bits::AssignBit(uint64_t field, int bitIdx, bool value) {
    if(bitIdx >= 64)
        return field;
    if(value)
        return SetBit(field, bitIdx);
    return ClearBit(field, bitIdx);
}

uint64_t Utils::Bits::SetBit(uint64_t field, int bitIdx) {
    if(bitIdx >= 64)
        return field;
    return field | (1 << bitIdx);
}

uint64_t Utils::Bits::ClearBit(uint64_t field, int bitIdx) {
    if(bitIdx >= 64)
        return field;
    return field & ~(1 << bitIdx);
}
