/* NumPy's own pcg64.h: the DXSM output and the cheap-multiplier step, pcg_cm_random_r. */
#include "common.h"
#include "ref/pcg64.h"
int main(void) {
    pcg_state_setseq_128 r;
    BENCH("pcg64dxsm", (r.state = ((__uint128_t)0x0123456789abcdefull << 64) | 0x0011223344556677ull,
                        r.inc = ((__uint128_t)0x0f1e2d3c4b5a6978ull << 64) | 0x8796a5b4c3d2e1f1ull),
          pcg_cm_random_r(&r));
}
