CC=gcc
FLAGS=-c -Wall -Werror -Wextra -std=c11

all: s21_grep

rebuild: clean all

test: s21_grep
	cp s21_grep test_grep
	sh grep_test_1.sh
	sh grep_test_2.sh

s21_grep: s21_grep.o
	${CC} s21_grep.o -o s21_grep

s21_grep.o: s21_grep.c
	${CC} s21_grep.c ${FLAGS} -o s21_grep.o

clean:
	rm -rf ./*.o ./s21_grep ./test_grep/s21_grep