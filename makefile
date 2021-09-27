CFLAGS = -Wall -Wextra -Isrc

all: shell379 runner sleeper

run: shell379
	./shell379

runner: util/runner.c
	gcc $(CFLAGS) -o $@ $<

sleeper: util/sleeper.c
	gcc $(CFLAGS) -o $@ $<

# no fancy make functions here
shell379: build/main.o build/process_table.o build/input_parser.o
	gcc $(CFLAGS) -o $@ $^ 

build/%.o: src/%.c build
	gcc $(CFLAGS) -o $@ -c $<

build:
	mkdir build

clean:
	rm shell379
	rm -rf build/*