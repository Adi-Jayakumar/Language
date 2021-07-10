#pragma once
#include <iostream>
#include <stdexcept>
#include <string>

struct Error : public std::exception
{
    std::string msg;
    Error(std::string s);
    const char *what() const throw()
    {
        return msg.c_str();
    }
    void Dump();
};

Error &operator+(Error &out, std::string s);