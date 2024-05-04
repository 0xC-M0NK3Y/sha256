#ifndef SHA256_H__
# define SHA256_H__

#include <stddef.h>
#include <stdint.h>

typedef struct sha256 {
    uint64_t size;
    uint32_t h0;
    uint32_t h1;
    uint32_t h2;
    uint32_t h3;
    uint32_t h4;
    uint32_t h5;
    uint32_t h6;
    uint32_t h7;
    uint8_t  tmp_buf[64];
    size_t   tmp_size;
} sha256_t;

#define SHA256_SIZE (256 / 8)

void sha256_init(sha256_t *ctx);
void sha256_update(sha256_t *ctx, const uint8_t *input, size_t input_size);
void sha256_finish(sha256_t *ctx, uint8_t hash[SHA256_SIZE]);

#endif /* sha256.h */
