#pragma once
#include <cstddef>

// Ids for serialising attributes of compilers
// and deserialising them into a VM
constexpr size_t INT_ID = 0xAAAAAAAAAAAAAAAA;
constexpr size_t DOUBLE_ID = 0xBBBBBBBBBBBBBBBB;
constexpr size_t BOOL_ID = 0xCCCCCCCCCCCCCCCC;
constexpr size_t CHAR_ID = 0xDDDDDDDDDDDDDDDD;
constexpr size_t STRING_ID = 0xEEEEEEEEEEEEEEEE;
constexpr size_t CODE_ID = 0xFFFFFFFFFFFFFFFF;
constexpr size_t STRUCT_TREE_ID = 0xABABABABABABABAB;
constexpr size_t LIB_FUNC_ID = 0xBCBCBCBCBCBCBCBC;
constexpr size_t THROW_INFO_ID = 0xCDCDCDCDCDCDCDCD;