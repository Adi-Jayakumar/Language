#include "../include/runtimeobject.h"
#include <cmath>

extern "C" constexpr const char *LibraryFunctions[]{"Sin                : double - double",
                                                    "Cos                : double - double",
                                                    "Tan                : double - double",
                                                    "GetPi              :        - double",
                                                    "DoNothing          : double - ",
                                                    "EuclideanDist      : double, double - double"};

extern "C" size_t NumLibFunctions = 6;

extern "C" Object *Sin(Object **args);
extern "C" Object *Cos(Object **args);
extern "C" Object *Tan(Object **args);
extern "C" Object *GetPi(Object **args);
extern "C" Object *DoNothing(Object **args);
extern "C" Object *EuclideanDist(Object **args);