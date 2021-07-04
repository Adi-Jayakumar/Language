#pragma once

#ifndef COMMON_H_INCLUDED
#include <cstdint>
typedef uint8_t TypeID;
#endif
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
    Object **arr;
    size_t size;
    Array(Object **_arr, size_t _size) : arr(_arr), size(_size){};
    virtual std::string ToString() override;
};

class Struct : public Object
{
public:
    Object **arr;
    size_t size;
    TypeID type;
    Struct(Object **_arr, size_t _size, TypeID _type) : arr(_arr), size(_size), type(_type){};
    virtual std::string ToString() override;
};

class Char : public Object
{
public:
    char c;
    Char(char _c) : c(_c){};
    virtual std::string ToString() override;
};

class String : public Object
{
public:
    char *str;
    size_t len;
    String(char *_str, size_t _len) : str(_str), len(_len){};
    virtual std::string ToString() override;
};

Object *CreateInt(int);
Object *CreateDouble(double);
Object *CreateArray(size_t, Object **);
Object *CreateStruct(size_t, Object **, TypeID);
Object *CreateChar(char);
Object *CreateString(char *);

// All getters assume that the underlying
// type of the Object* is appropriate
int GetInt(Object *);
double GetDouble(Object *);

size_t GetArrayLength(Object *);
Object **GetArray(Object *);

size_t GetNumStructMembers(Object *);
Object **GetStructMembers(Object *);
TypeID GetStructType(Object *);

char GetChar(Object *);
char *GetString(Object *);
