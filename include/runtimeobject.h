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
    virtual bool IsTruthy() { return false; }
};

class Int : public Object
{
public:
    int i;
    Int(int _i) : i(_i){};
    virtual std::string ToString() override;
    virtual bool IsTruthy() { return i != 0; };
};

class Double : public Object
{
public:
    double d;
    Double(double _d) : d(_d){};
    virtual std::string ToString() override;
    virtual bool IsTruthy() { return d != 0; };
};

class Bool : public Object
{
public:
    bool b;
    Bool(bool _b) : b(_b){};
    virtual std::string ToString() override;
    virtual bool IsTruthy() { return b; };
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

class Null_T : public Object
{
public:
    char isNull;
    Null_T() = default;
    virtual std::string ToString() override;
};

class LibraryFunctionDef
{
public:
    std::string name;
    std::string library;
    size_t arity;
    LibraryFunctionDef(std::string _name, std::string _library, size_t _arity) : name(_name), library(_library), arity(_arity){};
};

Object *CreateInt(int);
Object *CreateDouble(double);
Object *CreateBool(bool);
Object *CreateArray(Object **, size_t);
Object *CreateStruct(Object **, size_t, TypeID);
Object *CreateChar(char);
Object *CreateString(char *, size_t);
Object *CreateNull_T();

bool IsNull_T(Object *o);

// All getters assume that the underlying
// type of the Object* is appropriate
int GetInt(Object *);
double GetDouble(Object *);
bool GetBool(Object *);

size_t GetArrayLength(Object *);
Object **GetArray(Object *);

size_t GetNumStructMembers(Object *);
Object **GetStructMembers(Object *);
TypeID GetStructType(Object *);

char GetChar(Object *);

char *GetString(Object *);
size_t GetStringLen(Object *);
