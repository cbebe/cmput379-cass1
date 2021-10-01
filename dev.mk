CFLAGS = -Wall -Wextra
UTIL=runner sleeper piper logger

all: shell379 $(UTIL) input

run: shell379 $(UTIL)
	./shell379

input:
	echo "15\n200000000" >input

runner: util/runner.c
	gcc $(CFLAGS) -o $@ $<

piper: util/piper.c
	gcc $(CFLAGS) -o $@ $<

logger: util/logger.c
	gcc $(CFLAGS) -o $@ $<

sleeper: util/sleeper.c
	gcc $(CFLAGS) -o $@ $<

shell379: build/main.o build/process_table.o build/process.o build/input_parser.o
	gcc $(CFLAGS) -o $@ $^

build/%.o: src/%.c build
	gcc $(CFLAGS) -o $@ -c $<

build:
	mkdir build

clean:
	rm -f shell379 $(UTIL) input output
	rm -rf build
