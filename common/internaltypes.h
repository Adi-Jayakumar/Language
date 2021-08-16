#pragma once
#include <cstddef>
#include <cstdint>

using TypeID = uint8_t;

using oprand_t = uint16_t;
constexpr size_t MAX_OPCODE = UINT8_MAX;

using op_t = uint32_t;
constexpr size_t MAX_OP = UINT8_MAX;