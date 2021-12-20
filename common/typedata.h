#pragma once
#include "internaltypes.h"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

class TypeData
{
public:
    size_t is_array = 0;
    TypeID type = 0;
    TypeData() = default;
    TypeData(size_t _isArray, TypeID _type) : is_array(_isArray), type(_type){};
};

#define VOID_TYPE TypeData(0, 0)
#define INT_TYPE TypeData(0, 1)
#define DOUBLE_TYPE TypeData(0, 2)
#define BOOL_TYPE TypeData(0, 3)
#define STRING_TYPE TypeData(0, 4)
#define CHAR_TYPE TypeData(0, 5)
#define NULL_TYPE TypeData(0, 6)

constexpr size_t NUM_DEF_TYPES = 7UL;

constexpr size_t INT_SIZE = sizeof(int);
constexpr size_t DOUBLE_SIZE = sizeof(double);
constexpr size_t BOOL_SIZE = sizeof(bool);
constexpr size_t STRING_SIZE = INT_SIZE + sizeof(char *);
constexpr size_t CHAR_SIZE = sizeof(char);
constexpr size_t NULL_SIZE = 1UL;
constexpr size_t ARRAY_SIZE = sizeof(char *);
constexpr size_t STRUCT_SIZE = ARRAY_SIZE;
constexpr size_t PTR_SIZE = STRUCT_SIZE;

const std::vector<TypeData> AllTypes{VOID_TYPE, INT_TYPE, DOUBLE_TYPE, BOOL_TYPE, STRING_TYPE, CHAR_TYPE, NULL_TYPE};

#define VOID_ARRAY TypeData(1, 0)
#define INT_ARRAY TypeData(1, 1)
#define DOUBLE_ARRAY TypeData(1, 2)
#define BOOL_ARRAY TypeData(1, 3)
#define STRING_ARRAY TypeData(1, 4)
#define CHAR_ARRAY TypeData(1, 5)
#define NULL_ARRAY TypeData(1, 6)

bool operator==(const TypeData &left, const TypeData &right);
bool operator!=(const TypeData &left, const TypeData &right);
std::ostream &operator<<(std::ostream &out, const TypeData td);

namespace std
{
    template <>
    struct hash<TypeData>
    {
        size_t operator()(const TypeData &t) const
        {
            std::hash<size_t> st_hasher;
            std::hash<TypeID> type_id_hasher;
            return st_hasher(t.is_array) ^ type_id_hasher(t.type);
        }
    };
}