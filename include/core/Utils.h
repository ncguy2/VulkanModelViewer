//
// Created by Guy on 14/07/2021.
//

#ifndef GLMODELVIEWER_UTILS_H
#define GLMODELVIEWER_UTILS_H

#include <cstdint>
namespace Utils::Bits {
    bool GetBit(uint64_t field, int bitIdx);
    uint64_t AssignBit(uint64_t field, int bitIdx, bool value);
    uint64_t SetBit(uint64_t field, int bitIdx);
    uint64_t ClearBit(uint64_t field, int bitIdx);
}// namespace Utils::Bits

#endif//GLMODELVIEWER_UTILS_H
