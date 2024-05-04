PROG_SHA256   = bin/sha256
LIB_SHA256    = lib/sha256.a

LIB_SRC    = sha256.c
LIB_OBJ    = $(addprefix build/, $(LIB_SRC:.c=.o))

PROG_SRC   = main.c
PROG_OBJ   = $(addprefix build/, $(PROG_SRC:.c=.o))

CFLAGS     = -Wall -Wextra -g -masm=intel
CC         = gcc
AR         = ar rcs

all: $(LIB_SHA256) $(PROG_SHA256)

$(PROG_SHA256): $(LIB_SHA256) $(PROG_OBJ)
	mkdir -p $(shell dirname $(PROG_SHA256))
	$(CC) $(CFLAGS) $(PROG_OBJ) $(LIB_SHA256) -o $(PROG_SHA256)
$(LIB_SHA256): $(LIB_OBJ)
	mkdir -p $(shell dirname $(LIB_SHA256))
	$(AR) $(LIB_SHA256) $(LIB_OBJ)

build/%.o: %.c
	mkdir -p $(shell dirname $(@))
	$(CC) $(CFLAGS) -c $(<) -o $(@) -masm=intel

clean:
	rm -rf build/*
fclean: clean
	rm -rf $(PROG_SHA256)
	rm -rf $(LIB_SHA256)
re: fclean all

.phony: all clean fclean re
