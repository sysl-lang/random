/* mt19937ar.c, the authors' file, its demo main renamed away. A 64-bit output is two draws. */
#define main mt_demo_main
#include "ref/mt19937ar.c"
#undef main
#include "common.h"
/* The high draw first, sequenced: C leaves the order of two calls in one expression unspecified. */
static inline uint64_t two(void) { uint64_t h = genrand_int32(); return h << 32 | genrand_int32(); }
int main(void) { BENCH("mt19937", init_genrand(5489), two()); }
