#pragma once
#include <cstdint>
#include <sstream>

typedef uint8_t TypeID;
enum class GCState
{
    FREED,
    MARKED,
    UNMARKED,
};

class Object
{
public:
    GCState state = GCState::UNMARKED;

    virtual ~Object(){};
    virtual std::string ToString() = 0;

    // never to be overloaded by library type
    virtual bool IsTruthy() { return false; }

    // GC STUFF
    virtual void MarkChildren(){};
    virtual void DestroyOwnedMemory(){};
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

    virtual void MarkChildren() override
    {
        for (size_t i = 0; i < size; i++)
            arr[i]->MarkChildren();
    };
};

class Struct : public Object
{
public:
    Object **arr;
    size_t size;
    TypeID type;
    Struct(Object **_arr, size_t _size, TypeID _type) : arr(_arr), size(_size), type(_type){};
    virtual std::string ToString() override;

    virtual void MarkChildren() override
    {
        for (size_t i = 0; i < size; i++)
            arr[i]->MarkChildren();
    };
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

    virtual void DestroyOwnedMemory() override { delete[] str; };
};

class Null_T : public Object
{
public:
    char isNull;
    Null_T() = default;
    virtual std::string ToString() override;
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
void SetStructType(Object *, TypeID);

char GetChar(Object *);

char *GetString(Object *);
size_t GetStringLen(Object *);
