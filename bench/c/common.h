/* The timing harness every C reference shares with bench/sysl/main.sysl: N outputs summed (wrapping)
 * into a checksum, best of REPS runs, the state reset before each. The checksum is printed so the
 * sysl run and the C run can be seen to compute the same numbers, not merely similar amounts of work.
 * Prints: <name> <ns per output> <checksum>. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define N (1ull << 27)
#define REPS 7

static inline uint64_t now_ns(void) { return clock_gettime_nsec_np(CLOCK_UPTIME_RAW); }
static inline uint64_t fbits(double d) { uint64_t u; memcpy(&u, &d, 8); return u; }

#define BENCH(name, setup, expr)                                              \
    do {                                                                      \
        double best = 1e30;                                                   \
        uint64_t check = 0;                                                   \
        for (int rep = 0; rep < REPS; rep++) {                                \
            setup;                                                            \
            uint64_t acc = 0, t0 = now_ns();                                  \
            for (uint64_t i = 0; i < N; i++) acc += (expr);                   \
            uint64_t t1 = now_ns();                                           \
            double ns = (double)(t1 - t0) / (double)N;                        \
            if (ns < best) best = ns;                                         \
            check = acc;                                                      \
        }                                                                     \
        printf("%s %.4f %llu\n", name, best, (unsigned long long)check);      \
    } while (0)
