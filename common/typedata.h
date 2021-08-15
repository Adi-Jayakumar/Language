#include <cstddef>
#include <cstdint>

#define NUM_DEF_TYPES 7U
typedef uint8_t TypeID;
class TypeData
{
public:
    size_t isArray = 0;
    TypeID type = 0;
    TypeData() = default;
    TypeData(size_t _isArray, TypeID _type) : isArray(_isArray), type(_type){};
};

#define VOID_TYPE TypeData(0, 0)
#define INT_TYPE TypeData(0, 1)
#define DOUBLE_TYPE TypeData(0, 2)
#define BOOL_TYPE TypeData(0, 3)
#define STRING_TYPE TypeData(0, 4)
#define CHAR_TYPE TypeData(0, 5)
#define NULL_TYPE TypeData(0, 6)

#define VOID_ARRAY TypeData(1, 0)
#define INT_ARRAY TypeData(1, 1)
#define DOUBLE_ARRAY TypeData(1, 2)
#define BOOL_ARRAY TypeData(1, 3)
#define STRING_ARRAY TypeData(1, 4)
#define CHAR_ARRAY TypeData(1, 5)
#define NULL_ARRAY TypeData(1, 6)