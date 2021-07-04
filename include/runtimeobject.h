#pragma once
#include "common.h"
#include <sstream>

class Object
{
public:
    virtual ~Object(){};
    virtual std::string ToString() = 0;
    virtual void DestroyOwnedMemory(){};
};

class Int : public Object
{
public:
    int i;
    Int(int _i) : i(_i){};
    virtual std::string ToString() override;
};

class Double : public Object
{
public:
    double d;
    Double(double _d) : d(_d){};
    virtual std::string ToString() override;
};

class Array : public Object
{
public:
    size_t size;
    Object **arr;
    Array(size_t _size, Object **_arr) : size(size), arr(_arr){};
    virtual std::string ToString() override;
};

class Struct : public Object
{
public:
    size_t size;
    Object **arr;
    TypeID type;
    Struct(size_t _size, Object **_arr, TypeID _type) : size(size), arr(_arr), type(_type){};
    virtual std::string ToString() override;
};

Object *CreateInt(int);
Object *CreateDouble(double);
Object *CreateArray(size_t, Object **);
Object *CreateStruct(size_t, Object **, TypeID);