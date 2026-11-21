#define main mt_demo_main
#include "ref/mt19937-64.c"
#undef main
#include "common.h"
int main(void) { BENCH("mt19937_64", init_genrand64(5489), genrand64_int64()); }
