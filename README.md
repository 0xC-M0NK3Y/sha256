# C implementation of sha256

## Overview

Its a simple C sha256 implementation,  
It comes with a library, a binary, and a tester  
  
to compile it do  
```sh
  $ make
```
  
to use the library use the sha256.a in the lib folder  
its easy to use, like every implementation there is three functions (look sha256.h):  
  
```c
void sha256_init(sha256_t *ctx);
void sha256_update(sha256_t *ctx, const uint8_t *input, size_t input_size);
void sha256_finish(sha256_t *ctx, uint8_t hash[SHA256_SIZE]);
```
  
the binary sha256 in the bin folder take excactly one argument and computes the hash and prints it  
you can compare it to other sha256 implementations  
  
```sh
  $ ./bin/sha256 "0xC_M0NK3Y"
  c5b9259de99de0b4441c667d4c8b9cbfd073a9c231b15c8ed7cf414b8e905ad5
  $ echo -n "0xC_M0NK3Y" | sha256sum
  c5b9259de99de0b4441c667d4c8b9cbfd073a9c231b15c8ed7cf414b8e905ad5  -
```
  
you can also test my implementation with the tester.sh script  
take a look at it, you can change the number of tests and the max length of the strings tested  
it perfoms a diff on the results of my sha256 and sha256sum  
