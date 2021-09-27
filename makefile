all: shell379

shell379: build/main.o
	gcc -o $@ $<

build/%.o: src/%.c build
	gcc -o $@ -c $<

build:
	mkdir build