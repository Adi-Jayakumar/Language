#pragma once
#include <string>
#include <iostream>
#include <stdexcept>

struct Error
{
    std::string msg;
    Error(std::string s);
    void Dump();
};

Error &operator+(Error &out, std::string s);