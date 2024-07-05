SHELL := /bin/bash
FLAGS=-Wall -pedantic
OBJ=$(SRC.c=.o)
FILE=test_1_cat.txt

all: s21_cat

s21_cat: s21_cat.o
	gcc $(FLAGS) s21_cat.o -o s21_cat

s21_cat.o: s21_cat.c 
	gcc $(FLAGS) -c s21_cat.c

clean:
	rm -rf $(OBJ) s21_cat *.out *.log *.dSYM *.o

rebuild: clean all

testing:
	-diff <(./s21_cat $(FILE)) <(cat $(FILE))
	-diff <(./s21_cat -e $(FILE)) <(cat -ev $(FILE))
	-diff <(./s21_cat -b $(FILE)) <(cat -b $(FILE))
	-diff <(./s21_cat -n $(FILE)) <(cat -n $(FILE))
	-diff <(./s21_cat -s $(FILE)) <(cat -s $(FILE))
	-diff <(./s21_cat -t $(FILE)) <(cat -tv $(FILE))
	-diff <(./s21_cat --number $(FILE)) <(cat -n $(FILE))
	-diff <(./s21_cat --number-nonblank $(FILE)) <(cat -b $(FILE))
	-diff <(./s21_cat -T $(FILE)) <(cat -t $(FILE))

