// C++'s own engines, libc++: std::mt19937 (a 64-bit output is two draws) and std::mt19937_64.
#include <random>
extern "C" {
#include "common.h"
}
static std::mt19937 a;
static inline uint64_t two() { uint64_t h = a(); return h << 32 | a(); }
int main() {
    std::mt19937_64 b;
    BENCH("std::mt19937", a.seed(5489), two());
    BENCH("std::mt19937_64", b.seed(5489), b());
}
