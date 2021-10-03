CFLAGS=-Wall -Wextra
UTIL_SRC=runner sleeper piper logger writer
UTIL=$(UTIL_SRC:%=%.util)

SRC=main process_table process input_parser
OBJ=$(SRC:%=build/%.o)

BUILD=build

all: shell379 $(UTIL) input

run: shell379 $(UTIL)
	./shell379

input:
	echo "15\n2000000" >input

%.util: util/%.c
	gcc $(CFLAGS) -o $@ $<

shell379: $(OBJ)
	gcc $(CFLAGS) -o $@ $^

$(BUILD)/%.o: src/%.c $(BUILD)
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD):
	mkdir $<

clean:
	rm -f shell379 $(UTIL) input output
	rm -rf $(BUILD)
