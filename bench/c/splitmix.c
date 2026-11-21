#include "common.h"
#include "ref/splitmix64.c"
int main(void) { BENCH("splitmix64", x = 1, next()); }
