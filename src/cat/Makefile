CC=gcc
FLAGS=-c -Wall -Werror -Wextra -std=c11

all: s21_cat

rebuild: clean all

test: s21_cat
	sh tests.sh

s21_cat: s21_cat.o
	${CC} s21_cat.o -o s21_cat

s21_cat.o: s21_cat.c
	${CC} s21_cat.c ${FLAGS} -o s21_cat.o

clean:
	rm -rf ./*.o ./s21_cat