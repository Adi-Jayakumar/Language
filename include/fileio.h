#pragma once
#include <fstream>
#include <streambuf>
#include <string>

namespace IO
{
    std::string GetSrcString(std::string fName)
    {
        std::ifstream in = std::ifstream(fName);
        std::string src = std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        return src;
    }
}