#pragma once
#include <cstddef>
#include <cstdint>

using TypeID = uint8_t;
constexpr size_t MAX_TYPE = UINT8_MAX;

using oprand_t = size_t;
constexpr size_t MAX_OPRAND = SIZE_MAX;

using op_t = uint8_t;
constexpr size_t MAX_OP = UINT8_MAX;