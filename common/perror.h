#pragma once
#include <iostream>
#include <stdexcept>
#include <string>

struct Error : public std::exception
{
    std::string msg;

    Error() = default;

    Error(const std::string &_msg) : msg(_msg){};

    const char *what() const noexcept override
    {
        return msg.c_str();
    }
};

#define ERROR_GUARD(program, eh)            \
    try                                     \
        program catch (std::exception & e)  \
    {                                       \
        eh.had_error = true;                \
        std::cerr << e.what() << std::endl; \
    }
