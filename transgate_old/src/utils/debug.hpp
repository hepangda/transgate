// #define TRANSGATE_DEBUG
#include <unistd.h>
#include <iostream>
#include <string_view>

#ifdef TRANSGATE_DEBUG
#define tdbc(a) std::cerr << #a << ": " << a << std::endl;
#define DOUT(x) std::cerr << x
#define ENL << std::endl
#define DPRINT(...) printf(__VA_ARGS__)
#else
#define tdbc(a)
#define DOUT(x)
#define ENL
#define DPRINT(...)
#endif  // TRANSGATE_DEBUG