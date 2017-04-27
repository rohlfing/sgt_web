flags = -O3 -Wall -Werror

all: spectre

spectre: spectre.c graph.o
	gcc -static -o $@ spectre.c graph.o -lgsl -lgslcblas -lm $(flags)

graph.o: graph.c graph.h
	gcc -c graph.c $(flags)
