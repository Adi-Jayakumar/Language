#pragma once
#include "common.h"
#include "typedata.h"

std::unordered_map<TypeID, TypeData> GetTemplateMap();

struct TypeSubstitution
{
    TypeSubstitution();
    TypeSubstitution(Token _loc);
    TypeData operator[](TypeData type);
    // incoming type is a template type ie. Array<n, T>
    // for some dummy type T which will be contained in
    // 'map' and so this will map T's type to the required type
    // and return the required arrayed version (in case T is
    // mapped to Array<3, int> for example)

private:
    // a map from dummy types to real types
    // by default will map standard types to
    // themselves
    std::unordered_map<TypeID, TypeData> map;
    Token loc;
};