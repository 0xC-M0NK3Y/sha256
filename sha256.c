#include <stddef.h>
#include <stdint.h>

#include "sha256.h"

static const uint32_t K[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};
static uint8_t padding[] = {
	(1 << 7), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0,        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0,        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0,        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static inline size_t congruant_448_mod_512(size_t len) {
	if (len == 448 || len % 512 == 448)
		return 512;
	return ((448 - (len % 512) + 512) % 512);
}
static inline uint32_t right_rotate(uint32_t x, uint32_t n) {
	uint32_t r;
	asm ("mov %%eax, %[in1]\n"
		 "ror %%eax, %[in2]\n"
		 "mov %[out], %%eax\n"
		 : [out] "=r" (r)
		 : [in1] "r" (x), [in2] "r" (n)
		 : "eax"
	);
	return r;
}
#define SWAP_UINT64(x) \
    ((((x) & 0x00000000000000FFULL) << 56) | \
     (((x) & 0x000000000000FF00ULL) << 40) | \
     (((x) & 0x0000000000FF0000ULL) << 24) | \
     (((x) & 0x00000000FF000000ULL) << 8)  | \
     (((x) & 0x000000FF00000000ULL) >> 8)  | \
     (((x) & 0x0000FF0000000000ULL) >> 24) | \
     (((x) & 0x00FF000000000000ULL) >> 40) | \
     (((x) & 0xFF00000000000000ULL) >> 56))

#define SWAP_UINT32(x) \
    ((((x) & 0x000000FFU) << 24) | \
     (((x) & 0x0000FF00U) << 8)  | \
     (((x) & 0x00FF0000U) >> 8)  | \
     (((x) & 0xFF000000U) >> 24))

#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define SIGMA0(x) (right_rotate(x, 7) ^ right_rotate(x, 18) ^ (x >> 3))
#define SIGMA1(x) (right_rotate(x, 17) ^ right_rotate(x, 19) ^ (x >> 10))

#define EPSYLON0(x)                                                     \
    (right_rotate(x, 2) ^ right_rotate(x, 13) ^ right_rotate(x, 22))
#define EPSYLON1(x)                                                     \
    (right_rotate(x, 6) ^ right_rotate(x, 11) ^ right_rotate(x, 25))

void sha256_init(sha256_t *ctx) {
    ctx->size     = 0;
    ctx->h0       = 0x6a09e667;
    ctx->h1       = 0xbb67ae85;
    ctx->h2       = 0x3c6ef372;
    ctx->h3       = 0xa54ff53a;
    ctx->h4       = 0x510e527f;
    ctx->h5       = 0x9b05688c;
    ctx->h6       = 0x1f83d9ab;
    ctx->h7       = 0x5be0cd19;
    ctx->tmp_size = 0;
}

static void sha256_step(sha256_t *ctx, uint32_t *w) {
    uint32_t a, b, c, d, e, f, g, h;
    uint32_t t1;
    uint32_t t2;

    a = ctx->h0;
    b = ctx->h1;
    c = ctx->h2;
    d = ctx->h3;
    e = ctx->h4;
    f = ctx->h5;
    g = ctx->h6;
    h = ctx->h7;
    
    for (int i = 0; i < 64; i++) {
        t1 = h + EPSYLON1(e) + CH(e, f, g) + K[i] + w[i];
        t2 = EPSYLON0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    ctx->h0 += a;
    ctx->h1 += b;
    ctx->h2 += c;
    ctx->h3 += d;
    ctx->h4 += e;
    ctx->h5 += f;
    ctx->h6 += g;
    ctx->h7 += h;
}

void sha256_update(sha256_t *ctx, const uint8_t *input, size_t input_size) {
    ctx->size += input_size;
    
    for (size_t i = 0; i < input_size; i++) {
        ctx->tmp_buf[ctx->tmp_size] = input[i];
        ctx->tmp_size++;

        if (ctx->tmp_size >= 64) {
            uint32_t w[64];

            for (int j = 0; j < 16; j++)
                w[j] = SWAP_UINT32(((uint32_t *)ctx->tmp_buf)[j]);
            for (int j = 16; j < 64; j++)
                w[j] =
                    SIGMA1(w[j - 2]) + w[j - 7] + SIGMA0(w[j - 15]) + w[j - 16];
            sha256_step(ctx, w);
            ctx->tmp_size = 0;
        }
    }
}

void sha256_finish(sha256_t *ctx, uint8_t hash[SHA256_SIZE]) {
    uint8_t last_input[8];
    size_t  padding_len = congruant_448_mod_512(ctx->size * 8) / 8;
    
    *((uint64_t *)last_input) = SWAP_UINT64(ctx->size * 8);    
    
    sha256_update(ctx, padding, padding_len);
    sha256_update(ctx, last_input, sizeof(last_input));

    ((uint32_t *)hash)[0] = SWAP_UINT32(ctx->h0);
    ((uint32_t *)hash)[1] = SWAP_UINT32(ctx->h1);
    ((uint32_t *)hash)[2] = SWAP_UINT32(ctx->h2);
    ((uint32_t *)hash)[3] = SWAP_UINT32(ctx->h3);
    ((uint32_t *)hash)[4] = SWAP_UINT32(ctx->h4);
    ((uint32_t *)hash)[5] = SWAP_UINT32(ctx->h5);
    ((uint32_t *)hash)[6] = SWAP_UINT32(ctx->h6);
    ((uint32_t *)hash)[7] = SWAP_UINT32(ctx->h7); 
}
