#include "runtimeobject.h"

extern "C"
{
    const char *RuntimeTypeToString(const RuntimeType &rt)
    {
        switch (rt)
        {
        case RuntimeType::NULL_T:
        {
            return "NULL_T";
        }
        case RuntimeType::INT:
        {
            return "INT";
        }
        case RuntimeType::DOUBLE:
        {
            return "DOUBLE";
        }
        case RuntimeType::BOOL:
        {
            return "BOOL";
        }
        case RuntimeType::ARRAY:
        {
            return "ARRAY";
        }
        case RuntimeType::STRING:
        {
            return "STRING";
        }
        case RuntimeType::CHAR:
        {
            return "CHAR";
        }
        case RuntimeType::STRUCT:
        {
            return "STRUCT";
        }
        default:
        {
            return "";
        }
        }
    }

    const char *GCStateToString(const GCState &gcs)
    {
        switch (gcs)
        {
        case GCState::FREED:
        {
            return "FREED";
        }
        case GCState::MARKED:
        {
            return "MARKED";
        }
        case GCState::UNMARKED:
        {
            return "UNMARKED";
        }
        default:
        {
            return "";
        }
        }
    }

    struct RuntimeObject
    {
        RuntimeType type;
        GCState state = GCState::UNMARKED;
        union combo
        {
            int i;
            double d;
            bool b;
            char c;
            Array arr;
            String str;
        } as;
    };

    RuntimeObject *CreateRTOFromString(RuntimeType type, const char *literal)
    {
        RuntimeObject *res = (RuntimeObject *)malloc(sizeof(RuntimeObject));
        res->type = type;
        switch (type)
        {
        case RuntimeType::NULL_T:
        {
            res->as.i = 0;
            break;
        }
        case RuntimeType::INT:
        {
            res->as.i = atoi(literal);
            break;
        }
        case RuntimeType::DOUBLE:
        {
            res->as.d = atof(literal);
            break;
        }
        case RuntimeType::BOOL:
        {
            res->as.b = strcmp(literal, "true") == 0;
            break;
        }
        default:
        {
            break;
        }
        }
        return res;
    }

    RuntimeObject *CreateInt(int i)
    {
        RuntimeObject *res = (RuntimeObject *)malloc(sizeof(RuntimeObject));
        res->type = RuntimeType::INT;
        res->as.i = i;
        return res;
    }

    RuntimeObject *CreateDouble(double d)
    {
        RuntimeObject *res = (RuntimeObject *)malloc(sizeof(RuntimeObject));
        res->type = RuntimeType::DOUBLE;
        res->as.d = d;
        return res;
    }

    RuntimeObject *CreateBool(bool b)
    {
        RuntimeObject *res = (RuntimeObject *)malloc(sizeof(RuntimeObject));
        res->type = RuntimeType::BOOL;
        res->as.b = b;
        return res;
    }

    RuntimeObject *CreateArrayOrStruct(RuntimeType type, size_t size)
    {
        RuntimeObject *res = (RuntimeObject *)malloc(sizeof(RuntimeObject));
        res->type = type;
        res->as.arr.size = size;
        res->as.arr.data = (RuntimeObject **)malloc(sizeof(RuntimeObject *) * size);
        return res;
    }

    RuntimeObject *CreateString(String str)
    {
        RuntimeObject *res = (RuntimeObject *)malloc(sizeof(RuntimeObject));
        res->type = RuntimeType::STRING;
        res->as.str.len = str.len;
        char *copy = (char *)malloc(str.len + 1);
        strcpy(copy, str.data);
        res->as.str.data = copy;
        return res;
    }

    RuntimeObject *CreateChar(char c)
    {
        RuntimeObject *res = (RuntimeObject *)malloc(sizeof(RuntimeObject));
        res->type = RuntimeType::CHAR;
        res->as.c = c;
        return res;
    }

    RuntimeType GetType(RuntimeObject *rt)
    {
        return rt->type;
    }

    RuntimeObject *GetNull()
    {
        RuntimeObject *nullObj = (RuntimeObject *)malloc(sizeof(RuntimeObject));
        nullObj->type = RuntimeType::NULL_T;
        nullObj->as.i = 0;
        return nullObj;
    }

    RuntimeObject *CopyRTO(RuntimeObject *rt)
    {
        switch (rt->type)
        {
        case RuntimeType::NULL_T:
        {
            return GetNull();
        }
        case RuntimeType::INT:
        {
            return CreateInt(GetInt(rt));
        }
        case RuntimeType::DOUBLE:
        {
            return CreateDouble(GetDouble(rt));
        }
        case RuntimeType::BOOL:
        {
            return CreateBool(GetBool(rt));
        }
        case RuntimeType::STRUCT:
        case RuntimeType::ARRAY:
        {
            return CreateArrayOrStruct(rt->type, GetArrayOrStruct(rt).size);
        }
        case RuntimeType::STRING:
        {
            return CreateString(GetString(rt));
        }
        case RuntimeType::CHAR:
        {
            return CreateChar(GetChar(rt));
        }
        default:
        {
            return nullptr;
        }
        }
    }

    int GetInt(RuntimeObject *rt)
    {
        return rt->as.i;
    }
    double GetDouble(RuntimeObject *rt)
    {
        return rt->as.d;
    }
    bool GetBool(RuntimeObject *rt)
    {
        return rt->as.b;
    }
    Array GetArrayOrStruct(RuntimeObject *rt)
    {
        return rt->as.arr;
    }
    String GetString(RuntimeObject *rt)
    {
        return rt->as.str;
    }
    char GetChar(RuntimeObject *rt)
    {
        return rt->as.c;
    }

    RuntimeObject *SetInt(RuntimeObject *rt, int i)
    {
        rt->as.i = i;
        return rt;
    }

    RuntimeObject *SetDouble(RuntimeObject *rt, double d)
    {
        rt->as.d = d;
        return rt;
    }

    RuntimeObject *SetBool(RuntimeObject *rt, bool b)
    {
        rt->as.b = b;
        return rt;
    }

    RuntimeObject *SetArrayOrStruct(RuntimeObject *rt, Array arr)
    {
        rt->as.arr = arr;
        return rt;
    }

    RuntimeObject *SetIndexOfArray(RuntimeObject *arr, size_t index, RuntimeObject *val)
    {
        arr->as.arr.data[index] = val;
        return arr;
    }

    RuntimeObject *SetString(RuntimeObject *rt, String str)
    {
        rt->as.str = str;
        return rt;
    }

    RuntimeObject *SetChar(RuntimeObject *rt, char c)
    {
        rt->as.c = c;
        return rt;
    }

    GCState GetGCState(const RuntimeObject *rt)
    {
        return rt->state;
    }

    void SetGCState(RuntimeObject *rt, GCState state)
    {
        rt->state = state;
    }

    void InsertString(char *whole, const char *str, size_t len, size_t index)
    {
        for (size_t i = 0; i < len; i++)
        {
            whole[index + i] = str[i];
        }
    }

    char *RTOToString(RuntimeObject *rt)
    {
        switch (rt->type)
        {
        case RuntimeType::NULL_T:
        {
            return strdup("NULL");
        }
        case RuntimeType::INT:
        {
            size_t length = snprintf(NULL, 0, "%i", rt->as.i);
            char *str = (char *)malloc((length + 1) * sizeof(char));
            sprintf(str, "%i", rt->as.i);
            return str;
        }
        case RuntimeType::DOUBLE:
        {
            size_t length = snprintf(NULL, 0, "%f", rt->as.d);
            char *str = (char *)malloc((length + 1) * sizeof(char));
            sprintf(str, "%f", rt->as.d);
            return str;
        }
        case RuntimeType::BOOL:
            return rt->as.b ? strdup("true") : strdup("false");
        case RuntimeType::STRUCT:
        case RuntimeType::ARRAY:
        {
            Array arr = rt->as.arr;
            char **strs = (char **)malloc(arr.size * sizeof(char *));

            size_t arrStringLength = 0;
            for (size_t i = 0; i < arr.size; i++)
            {
                strs[i] = RTOToString(arr.data[i]);
                arrStringLength += strlen(strs[i]);
            }

            arrStringLength += 2 + 2 * (arr.size - 1) + 1;
            char *res = NULL;
            free(res);
            res = (char *)malloc(arrStringLength * sizeof(char));

            res[arrStringLength - 1] = '\0';
            res[0] = '{';

            size_t insertIndex = 1;

            for (size_t i = 0; i < arr.size; i++)
            {
                InsertString(res, strs[i], strlen(strs[i]), insertIndex);
                insertIndex += strlen(strs[i]);

                if (i != arr.size - 1)
                {
                    InsertString(res, ", ", 2, insertIndex);
                    insertIndex += 2;
                }
            }

            res[arrStringLength - 2] = '}';

            for (size_t i = 0; i < arr.size; i++)
                free(strs[i]);

            free(strs);

            return res;
        }
        case RuntimeType::STRING:
        {
            String str = GetString(rt);
            char *res = (char *)malloc((str.len + 1) * sizeof(char));
            return strcpy(res, str.data);
        }
        case RuntimeType::CHAR:
        {
            char *str = (char *)malloc(2);
            str[0] = GetChar(rt);
            return str;
        }
        default:
        {
            return strdup("");
        }
        }
    }

    bool IsTruthy(const RuntimeObject *rt)
    {
        switch (rt->type)
        {
        case RuntimeType::INT:
        {
            return rt->as.i != 0;
        }
        case RuntimeType::DOUBLE:
        {
            return rt->as.d != 0;
        }
        case RuntimeType::BOOL:
        {
            return rt->as.b;
        }
        default:
        {
            return false;
        }
        }
    }
}

std::ostream &operator<<(std::ostream &out, const RuntimeType &rtt)
{
    out << RuntimeTypeToString(rtt);
    return out;
}
std::ostream &operator<<(std::ostream &out, const GCState &gcs)
{
    out << GCStateToString(gcs);
    return out;
}