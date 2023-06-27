
FLAGS = -Wall -Wextra -Wno-unused-result -Wno-unused-parameter -g

DEPS = horario.h
OBJ = horario.o main.o

LIBED_DEPS = $(patsubst %.c,%.o,$(wildcard ./src/ed/*.c)) 

all: main

%.o: %.c %.h
	gcc $(FLAGS) -c -o $@ $< 

libed.a: $(LIBED_DEPS)
	ar -crs libed.a $(LIBED_DEPS)

main: main.c libed.a
	gcc $(FLAGS) -o main main.c -I src/search -I src/ed -L . -led -lm

clean:
	rm -f main libed.a  $(LIBED_DEPS)
	
run:
	./main

