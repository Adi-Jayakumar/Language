#pragma once
#include "perror.h"
#include <cstring>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class ArgParser
{
    std::unordered_map<std::string, std::string> argVals;
    std::unordered_set<std::string> switches;
    std::unordered_set<std::string> onSwitches;

    void ArgumentError(std::string msg)
    {
        Error e = Error("\n[ARGUMENT ERROR]: " + msg + "\n");
        throw e;
    }

public:
    ArgParser() = default;

    void AddArg(const std::vector<std::string> args)
    {
        for (const auto &arg : args)
            argVals[arg] = "";
    }

    std::string &GetArgVal(const std::string arg)
    {
        if (argVals.find(arg) == argVals.end())
            ArgumentError("'" + arg + "' is not a value taking argument");

        return argVals[arg];
    }

    void SetArgVal(const std::string arg, const std::string val)
    {
        if (IsArg(arg))
            argVals[arg] = val;
        else
            ArgumentError("'" + arg + "' is not a value taking argument");
    }

    bool IsArg(const std::string a)
    {
        return argVals.find(a) != argVals.end();
    }

    void AddSwitch(const std::string s)
    {
        switches.insert(s);
    }

    void TurnSwitchOn(const std::string s)
    {
        onSwitches.insert(s);
    }

    bool IsSwitch(const std::string s)
    {
        return switches.find(s) != switches.end();
    }

    bool IsSwitchOn(const std::string s)
    {
        return onSwitches.find(s) != switches.end();
    }

    void ParseArgs(int argc, char **argv)
    {
        std::vector<std::string> args;
        for (int i = 0; i < argc; i++)
        {
            int len = std::strlen(argv[i]);
            std::string sArg = std::string(argv[i], len);
            args.push_back(sArg);
        }

        size_t counter = 0;

        while (counter < args.size())
        {
            if (IsArg(args[counter]))
            {
                SetArgVal(args[counter], args[counter + 1]);
                counter += 2;
            }
            else if (IsSwitch(args[counter]))
            {
                TurnSwitchOn(args[counter]);
                counter++;
            }
            else
                ArgumentError("Unkown argument '" + args[counter] + "'");
        }
    }
};