#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_

#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

static const char ALLOWED_OPTIONS_SHORT[] = "AbeEnstTuv";
static const struct option ALLOWED_OPTIONS_LONG[] = {
    {"number-nonblank", 0, NULL, 'b'},
    {"number", 0, NULL, 'n'},
    {"squeeze-blank", 0, NULL, 's'},
    {NULL, 0, NULL, 0}};

typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int u;
  int v;
} options;

void set_options(char c, options* Options);
void read_options(int argc, char* argv[], options* Options);
void work_with_files(int files_count, char* files_path[], options* Options);
void output_file(FILE* file, options* Options);
void output_err_file(char* files_path);
void suppression(FILE* file, char previous_symbol, char current_symbol);
int number_lines_notempty(char previous_symbol, char current_symbol,
                          int str_number);
int number_lines_all(char previous_symbol, int str_number);
void endline_dollar(char previous_symbol, char current_symbol,
                    options* Options);
void show_nonprinting_cntrl(char current_symbol);

#endif