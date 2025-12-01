CC := gcc
CFLAGS := -std=gnu99 -Wall -Wextra

all: barrier simulador

barrier: build/main_barrier.o build/reusable_barrier.o
	$(CC) -o $@ $^ -pthread

simulador: build/main_sim.o build/simulador.o
	$(CC) -o $@ $^ -lm

build/%.o: src/%.c | build/
	$(CC) -o $@ -c $^ $(CFLAGS)

build/reusable_barrier.o: src/reusable_barrier.c
	$(CC) -o $@ -c $^ $(CFLAGS) -pthread

build/:
	mkdir -p build

clean:
	rm -f barrier simulador
