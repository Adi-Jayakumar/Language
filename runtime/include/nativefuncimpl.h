#pragma once
#include "typedata.h"
#include <iostream>

struct ReturnValue
{
    char *data;
    size_t size;

    ReturnValue(char *_data, const size_t _size) : data(_data), size(_size){};
};

bool operator==(const ReturnValue &lhs, const ReturnValue &rhs);
bool operator!=(const ReturnValue &lhs, const ReturnValue &rhs);

const ReturnValue NULL_RETURN(nullptr, 0);

ReturnValue PrintInt(char *x);
ReturnValue PrintDouble(char *x);
ReturnValue PrintBool(char *x);
ReturnValue PrintString(char *x);
ReturnValue PrintChar(char *x);
ReturnValue Length(char *x);