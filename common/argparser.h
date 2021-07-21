#include <string>
#include <unordered_map>

class ArgParser
{
    std::unordered_map<std::string, std::string> argVals;

public:
    void AddArg(std::string arg);
    std::string operator[](const std::string);

    ArgParser(int argc, char **argv)
    {
        for (int i = 0; i < argc; i++)
        {
            // if(char)
        }
    }
};