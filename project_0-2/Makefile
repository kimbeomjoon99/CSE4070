cc = gcc
cflags = -g
target = testlib
objects = bitmap.o hash.o list.o debug.o hex_dump.o main.o

$(target): $(objects)
	$(cc) -o $@ $(objects) $(cflags)

bitmap.o: bitmap.h limits.h round.h hex_dump.h bitmap.c
hash.o: hash.h hash.c
list.o: list.h list.c
debug.o: debug.h debug.c
hex_dump.o: hex_dump.h round.h hex_dump.c
main.o: bitmap.h hash.h list.h main.c

.PHONY : clean
clean :
	rm $(target) $(objects)

