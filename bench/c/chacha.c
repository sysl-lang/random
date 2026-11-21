/* ChaCha with Bernstein's chacha-ref.c core -- its QUARTERROUND macro and round loop verbatim, the
 * round count a parameter -- behind the same 8-word buffer the sysl generator keeps. */
#include "common.h"
#define ROTATE(v, c) (((v) << (c)) | ((v) >> (32 - (c))))
#define XOR(v, w) ((v) ^ (w))
#define PLUS(v, w) ((uint32_t)((v) + (w)))
#define QUARTERROUND(a,b,c,d) \
  x[a] = PLUS(x[a],x[b]); x[d] = ROTATE(XOR(x[d],x[a]),16); \
  x[c] = PLUS(x[c],x[d]); x[b] = ROTATE(XOR(x[b],x[c]),12); \
  x[a] = PLUS(x[a],x[b]); x[d] = ROTATE(XOR(x[d],x[a]), 8); \
  x[c] = PLUS(x[c],x[d]); x[b] = ROTATE(XOR(x[b],x[c]), 7);

typedef struct { uint32_t input[16]; uint64_t buf[8]; size_t at; } chacha;

static void refill(chacha *c) {
    uint32_t x[16];
    int i;
    for (i = 0; i < 16; ++i) x[i] = c->input[i];
    for (i = ROUNDS; i > 0; i -= 2) {
        QUARTERROUND( 0, 4, 8,12)
        QUARTERROUND( 1, 5, 9,13)
        QUARTERROUND( 2, 6,10,14)
        QUARTERROUND( 3, 7,11,15)
        QUARTERROUND( 0, 5,10,15)
        QUARTERROUND( 1, 6,11,12)
        QUARTERROUND( 2, 7, 8,13)
        QUARTERROUND( 3, 4, 9,14)
    }
    for (i = 0; i < 16; ++i) x[i] = PLUS(x[i], c->input[i]);
    for (i = 0; i < 8; ++i) c->buf[i] = (uint64_t)x[2 * i] | (uint64_t)x[2 * i + 1] << 32;
    if (++c->input[12] == 0) ++c->input[13];
    c->at = 0;
}

static inline uint64_t next(chacha *c) {
    if (c->at == 8) refill(c);
    return c->buf[c->at++];
}

static void keyed(chacha *c) {
    static const uint32_t sigma[4] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};
    for (int i = 0; i < 4; i++) c->input[i] = sigma[i];
    for (int i = 0; i < 8; i++) c->input[4 + i] = 0x03020100u + 0x04040404u * i;
    c->input[12] = c->input[13] = c->input[14] = c->input[15] = 0;
    c->at = 8;
}

int main(void) { chacha c; BENCH(NAME, keyed(&c), next(&c)); }
