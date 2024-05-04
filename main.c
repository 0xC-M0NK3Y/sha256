#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "sha256.h"

void print_hash(uint8_t *p){
    for(unsigned int i = 0; i < 32; ++i){
        printf("%02x", p[i]);
    }
    printf("\n");
}

int main(int argc, char **argv) {

	uint8_t	 hash[SHA256_SIZE];
	sha256_t ctx;

	if (argc != 2) {
		printf("Usage %s <string>\n", argv[0]);
		return 1;
	}
	sha256_init(&ctx);
	sha256_update(&ctx, (const uint8_t *)argv[1], strlen(argv[1]));
	sha256_finish(&ctx, hash);

	print_hash(hash);

	return 0;
}
