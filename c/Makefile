all: spectre

spectre: spectre.c graph.o
	gcc -static -o $@ spectre.c graph.o -lgsl -lgslcblas -lm -O3 

graph.o: graph.c graph.h
	gcc -c graph.c
