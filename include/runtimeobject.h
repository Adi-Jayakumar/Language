#pragma once
#include "common.h"
#include <cstring>

extern "C"
{

    struct RuntimeObject;
    struct RTArray
    {
        size_t size;
        RuntimeObject **data;
    };

    struct RTString
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

    RuntimeObject *CreateRTOLiteral(RuntimeType, const char *);

    RuntimeObject *CreateInt(int);
    RuntimeObject *CreateDouble(double);
    RuntimeObject *CreateBool(bool);
    // array case
    RuntimeObject *CreateArrayOrStruct(RuntimeType, size_t);
    // string case
    RuntimeObject *CreateString(RTString);
    // char case
    RuntimeObject *CreateChar(char);

    RuntimeObject *GetNull();

    // returns the copy
    RuntimeObject *CopyRTO(RuntimeObject *rt);

    int GetInt(RuntimeObject *);
    double GetDouble(RuntimeObject *);
    bool GetBool(RuntimeObject *);
    RTArray GetArrayOrStruct(RuntimeObject *);
    RTString GetString(RuntimeObject *);
    char GetChar(RuntimeObject *);

    RuntimeObject *SetInt(RuntimeObject *, int);
    RuntimeObject *SetDouble(RuntimeObject *, double);
    RuntimeObject *SetBool(RuntimeObject *, bool);
    RuntimeObject *SetArrayOrStruct(RuntimeObject *, RTArray);
    RuntimeObject *SetString(RuntimeObject *, RTString);
    RuntimeObject *SetChar(RuntimeObject *, char);

    GCState GetGCState(const RuntimeObject *);
    void SetGCState(RuntimeObject *, GCState);

    char *ToString(const RuntimeObject *cc);
    bool IsTruthy(const RuntimeObject *cc);
}

std::ostream &operator<<(std::ostream &out, const RuntimeObject &cc);
std::ostream &operator<<(std::ostream &out, const RuntimeType &gcs);
std::ostream &operator<<(std::ostream &out, const GCState &gcs);