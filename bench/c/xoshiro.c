/* xoshiro256++ or ** (XO selects the authors' file), and the samplers over xoshiro256++. */
#include "common.h"
#include <math.h>
#include XO
#include "../ziggurat.h"

static void seed(uint64_t z) {
    for (int i = 0; i < 4; i++) {
        z += 0x9e3779b97f4a7c15;
        uint64_t q = z;
        q = (q ^ (q >> 30)) * 0xbf58476d1ce4e5b9;
        q = (q ^ (q >> 27)) * 0x94d049bb133111eb;
        s[i] = q ^ (q >> 31);
    }
}

/* Lemire's nearly-divisionless bound, from the paper's listing. */
static inline uint64_t below(uint64_t n) {
    __uint128_t m = (__uint128_t)next() * n;
    uint64_t l = (uint64_t)m;
    if (l < n) {
        uint64_t t = -n % n;
        while (l < t) { m = (__uint128_t)next() * n; l = (uint64_t)m; }
    }
    return m >> 64;
}

static inline double unit(void) { return (next() >> 11) * 0x1.0p-53; }
static inline double unit_open_low(void) { return ((next() >> 11) + 1) * 0x1.0p-53; }

static inline double normal(void) {
    for (;;) {
        uint64_t bits = next();
        size_t i = bits & 0xff;
        double u = (bits >> 11) * 0x1.0p-52 - 1.0;
        double x = u * ZIG_NORM_X[i];
        if (fabs(x) < ZIG_NORM_X[i + 1]) return x;
        if (i == 0) {
            double tx = 1.0, ty = 0.0;
            while (-2.0 * ty < tx * tx) { tx = log(unit_open_low()) / ZIG_NORM_R; ty = log(unit_open_low()); }
            return u < 0.0 ? tx - ZIG_NORM_R : ZIG_NORM_R - tx;
        }
        if (ZIG_NORM_F[i + 1] + (ZIG_NORM_F[i] - ZIG_NORM_F[i + 1]) * unit() < exp(-0.5 * x * x)) return x;
    }
}

static inline double exponential(void) {
    for (;;) {
        uint64_t bits = next();
        size_t i = bits & 0xff;
        double x = (bits >> 11) * 0x1.0p-53 * ZIG_EXP_X[i];
        if (x < ZIG_EXP_X[i + 1]) return x;
        if (i == 0) return ZIG_EXP_R - log(unit_open_low());
        if (ZIG_EXP_F[i + 1] + (ZIG_EXP_F[i] - ZIG_EXP_F[i + 1]) * unit() < exp(-x)) return x;
    }
}

int main(void) {
    volatile uint64_t bound = 1000000007;
    uint64_t n = bound;
#ifdef SAMPLERS
    BENCH("below", seed(1), below(n));
    BENCH("unit", seed(1), fbits(unit()));
    BENCH("normal", seed(1), fbits(normal()));
    BENCH("exponential", seed(1), fbits(exponential()));
#else
    BENCH(NAME, seed(1), next());
#endif
    (void)n;
}
