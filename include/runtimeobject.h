#pragma once
#include "common.h"
#include <cstring>

extern "C"
{

    struct RuntimeObject;
    struct Array
    {
        size_t size;
        RuntimeObject **data;
    };

    struct String
    {
        size_t len;
        char *data;
    };

    enum class RuntimeType : uint8_t
    {
        NULL_T,
        INT,
        DOUBLE,
        BOOL,
        ARRAY,
        STRING,
        CHAR,
        STRUCT,
    };
    const char *RuntimeTypeToString(const RuntimeType &);

    enum class GCState : uint8_t
    {
        FREED,
        MARKED,
        UNMARKED,
    };
    const char *GCStateToString(const GCState &rts);

    RuntimeObject *CreateRTOFromString(RuntimeType, const char *);

    RuntimeObject *CreateNull();
    RuntimeObject *CreateInt(int);
    RuntimeObject *CreateDouble(double);
    RuntimeObject *CreateBool(bool);
    // array case
    RuntimeObject *CreateArrayOrStruct(RuntimeType, size_t);
    // string case
    RuntimeObject *CreateString(String);
    // char case
    RuntimeObject *CreateChar(char);

    RuntimeObject *GetNull();

    // returns the copy
    RuntimeObject *CopyRTO(RuntimeObject *rt);

    int GetInt(RuntimeObject *);
    double GetDouble(RuntimeObject *);
    bool GetBool(RuntimeObject *);
    Array GetArrayOrStruct(RuntimeObject *);
    String GetString(RuntimeObject *);
    char GetChar(RuntimeObject *);

    RuntimeType GetType(RuntimeObject *);

    RuntimeObject *SetInt(RuntimeObject *, int);
    RuntimeObject *SetDouble(RuntimeObject *, double);
    RuntimeObject *SetBool(RuntimeObject *, bool);
    RuntimeObject *SetArrayOrStruct(RuntimeObject *, Array);
    RuntimeObject *SetIndexOfArray(RuntimeObject *arr, size_t index, RuntimeObject *val);
    RuntimeObject *SetString(RuntimeObject *, String);
    RuntimeObject *SetChar(RuntimeObject *, char);

    GCState GetGCState(const RuntimeObject *);
    void SetGCState(RuntimeObject *, GCState);

    void InsertString(char *whole, const char *str, size_t len, size_t index);
    char *RTOToString(RuntimeObject *);
    bool IsTruthy(const RuntimeObject *);

    struct LibInfo
    {
        size_t size;
        char** info;
    };

    void FreeLibInfo(LibInfo);
}

std::ostream &operator<<(std::ostream &out, const RuntimeType &rtt);
std::ostream &operator<<(std::ostream &out, const GCState &gcs);