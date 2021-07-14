#pragma once
#include <iostream>
#include <stdexcept>
#include <string>

struct Error : public std::exception
{
    std::string msg;
    Error(std::string s)
    {
        msg = s;
    }

    const char *what() const throw()
    {
        return msg.c_str();
    }
    void Dump()
    {
        std::cout << msg << std::endl;
        exit(3);
    }

    void operator+(std::string s)
    {
        msg += s;
    }
};
