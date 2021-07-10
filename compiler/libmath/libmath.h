#include "../include/runtimeobject.h"
#include "../include/vm.h"
#include <cmath>

extern "C" constexpr const char *LibraryFunctions[]{"Sin                : double - double",
                                                    "Cos                : double - double",
                                                    "Tan                : double - double",
                                                    "GetPi              :        - double",
                                                    "DoNothing          : double - ",
                                                    "EuclideanDist      : double, double - double"};

extern "C" constexpr size_t NumLibFunctions{6};

extern "C" Object *Sin(VM *vm, Object **args);
extern "C" Object *Cos(VM *vm, Object **args);
extern "C" Object *Tan(VM *vm, Object **args);
extern "C" Object *GetPi(VM *vm, Object **args);
extern "C" Object *DoNothing(VM *vm, Object **args);
extern "C" Object *EuclideanDist(VM *vm, Object **args);