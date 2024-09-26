#ifndef S21_GREP_H
#define S21_GREP_H

#include <ctype.h>
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char SHORTOPTS[] = "e:f:isvnholc";

typedef struct {
  int size;
  regex_t* reg_data;
} Patterns;

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  Patterns patterns;
} options;

void work_with_files(int files_count, char* files_path[], options* Options);
void output_err_file(char* files_path);
void read_options(int argc, char* argv[], options* Options);
void set_options(char c, options* Options);
void init_patterns(Patterns* patterns);
void patterns_add_file(options* Options, char* optarg);
void work_with_file(FILE* file, options* Options, int files_count,
                    char* files_path);
void patterns_add_console(options* Options, char* optarg);
char* read_str(FILE* fp, char* buffer);
void opt_c(options* Options, int all_files_count, int counter_coincidence,
           int counter_line, char* files_path);
int opt_v(options* Options, char* file_path, int trigger_v, int all_files_count,
          int counter_line, char* buffer);
int opt_o(options* Options, char* line_ptr, regmatch_t match, char* file_path,
          int all_files_count, int counter_line, char* buffer, int i);
void name_number(options* Options, int all_files_count, char* file_path,
                 int counter_line);

void* safe_malloc(const size_t size);
void* safe_realloc(void* ptr, size_t size);
FILE* safe_fopen(const char* filename, char* modes, options* Options);

#endif