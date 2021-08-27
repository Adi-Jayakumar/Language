#include "../include/runtimeobject.h"
#include "../include/vm.h"
#include <chrono>

extern "C" constexpr const char *LibraryFunctions[]{"StartTime                :  - int",
                                                    "EndTime                  :  - int"};

extern "C" constexpr size_t NumLibFunctions{6};
