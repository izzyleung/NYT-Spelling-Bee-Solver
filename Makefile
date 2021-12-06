CC ?= clang
CFLAGS = -Wall -Wextra -std=c11 -O2
OUT = solver

all: format main

main: main.c
	$(CC) $(CFLAGS) main.c -o $(OUT)

format: main.c
	clang-format -i main.c

clean:
	rm -f $(OUT)
