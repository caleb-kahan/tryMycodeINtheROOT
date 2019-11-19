all: main.c header.h
	gcc -o program main.c
run:
	./program
clean:
	rm *.o
	rm *.~
