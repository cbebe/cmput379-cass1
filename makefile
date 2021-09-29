CFLAGS = -Wall -Wextra -Isrc
APP=shell379
OBJ=main.o process_table.o process.o input_parser.o

$(APP): $(OBJ)
	gcc $(CFLAGS) -o $@ $^

%.o: src/%.c
	gcc $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(APP) $(OBJ)