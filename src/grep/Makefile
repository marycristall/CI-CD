SHELL := /bin/bash
CC = gcc
CFLAGS = -Wall -pedantic
OBJ=$(SRC.c=.o)
FILE=test_grep.txt

all: s21_grep

s21_grep: s21_grep.o
	gcc $(CFLAGS) s21_grep.o -o s21_grep

s21_cat.o: s21_grep.c 
	gcc $(CFLAGS) -c s21_grep.c

clean:
	rm -rf $(OBJ) s21_grep *.out *.log *.dSYM *.o

rebuild: clean all

	