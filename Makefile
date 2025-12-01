CC := gcc
CFLAGS := -std=gnu99 -Wall -Wextra
LDFLAGS := -pthread -lm

all: barrier sim

barrier: build/main_barrier.o build/reusable_barrier.o
	$(CC) -o $@ $^ $(LDFLAGS)

sim: build/main_sim.o build/simulador.o
	$(CC) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c | build/
	$(CC) -o $@ -c $^ $(CFLAGS)

build/:
	mkdir -p build

clean:
	rm -f barrier sim
