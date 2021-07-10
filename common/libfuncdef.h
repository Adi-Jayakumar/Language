#include <string>
class LibraryFunctionDef
{
public:
    std::string name;
    std::string library;
    size_t arity;
    LibraryFunctionDef(std::string _name, std::string _library, size_t _arity) : name(_name), library(_library), arity(_arity){};
};

