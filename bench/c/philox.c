/* Philox4x32-10 from Random123's philox.h: _philox4x32round and _philox4x32bumpkey as the macros
 * expand them, behind the same two-word buffer and 128-bit counter the sysl generator keeps. */
#include "common.h"
typedef struct { uint32_t ctr[4], key[2]; uint64_t buf[2]; size_t at; } philox;

static inline void refill(philox *p) {
    uint32_t c0 = p->ctr[0], c1 = p->ctr[1], c2 = p->ctr[2], c3 = p->ctr[3], k0 = p->key[0], k1 = p->key[1];
    for (int r = 0; r < 10; r++) {
        if (r > 0) { k0 += 0x9E3779B9; k1 += 0xBB67AE85; }
        uint64_t p0 = (uint64_t)0xD2511F53 * c0, p1 = (uint64_t)0xCD9E8D57 * c2;
        uint32_t n0 = (uint32_t)(p1 >> 32) ^ c1 ^ k0, n1 = (uint32_t)p1, n2 = (uint32_t)(p0 >> 32) ^ c3 ^ k1, n3 = (uint32_t)p0;
        c0 = n0; c1 = n1; c2 = n2; c3 = n3;
    }
    p->buf[0] = (uint64_t)c0 | (uint64_t)c1 << 32;
    p->buf[1] = (uint64_t)c2 | (uint64_t)c3 << 32;
    for (int i = 0; i < 4; i++) if (++p->ctr[i] != 0) break;
    p->at = 0;
}

static inline uint64_t next(philox *p) {
    if (p->at == 2) refill(p);
    return p->buf[p->at++];
}

int main(void) {
    philox p;
    BENCH("philox4x32", (memset(&p, 0, sizeof p), p.key[0] = 1, p.at = 2), next(&p));
}
