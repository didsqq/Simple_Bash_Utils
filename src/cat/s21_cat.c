#include "s21_cat.h"

int main(int argc, char* argv[]) {
  options Options = {0};
  read_options(argc, argv, &Options);
  work_with_files(argc - optind, argv + optind, &Options);
  return 0;
}

int number_lines_notempty(char previous_symbol, char current_symbol,
                          int str_number) {  //-b нумерует непустые строки
  if (previous_symbol == '\n' && current_symbol != '\n') {
    printf("%6d\t", str_number++);
  }
  return str_number;
}

int number_lines_all(char previous_symbol,
                     int str_number) {  //-n нумирует все строки
  if (previous_symbol == '\n') {
    printf("%6d\t", str_number++);
  }
  return str_number;
}

void suppression(FILE* file, char previous_symbol,
                 char current_symbol) {  //-s сжимает строки
  if (previous_symbol == '\n' && current_symbol == '\n') {
    char symbol = getc(file);
    while (symbol == '\n') symbol = getc(file);
    ungetc(symbol, file);
  }
}

void endline_dollar(
    char previous_symbol, char current_symbol,
    options* Options) {  //-e отображает символы конца строки как $
  if (current_symbol == '\n' && previous_symbol == '\n' && (*Options).b)
    printf("      \t$");
  else if (current_symbol == '\n')
    putc('$', stdout);
}

void show_nonprinting_cntrl(char current_symbol) {
  if (iscntrl(current_symbol) && current_symbol != '\t' &&
      current_symbol != '\n') {
    putc('^', stdout);
    if (current_symbol == 127)
      putc((current_symbol - 64), stdout);
    else
      putc((current_symbol + 64), stdout);
  }
}

void print_meta(char current_symbol) {
  if ((signed char)current_symbol < 0) {
    char meta_symbol = (signed char)current_symbol + 127 + 1;
    putc('M', stdout);
    putc('-', stdout);
    if (meta_symbol == '\t') {
      putc('^', stdout);
      putc('I', stdout);
    } else if (meta_symbol == '\n') {
      putc('^', stdout);
      putc('J', stdout);
    } else {
      show_nonprinting_cntrl(meta_symbol);
      if (isprint(meta_symbol) || meta_symbol == '\n')
        putc((meta_symbol), stdout);
    }
  }
}

void print_symbols(char current_symbol, options* Options) {
  if ((*Options).t && current_symbol == '\t') {
    putc('^', stdout);
    putc('I', stdout);
  } else if (current_symbol == '\t') {
    putc('\t', stdout);
  } else if ((*Options).v) {
    show_nonprinting_cntrl(current_symbol);
    print_meta(current_symbol);
    if (isprint(current_symbol) || current_symbol == '\n')
      putc((current_symbol), stdout);
  } else {
    putc((current_symbol), stdout);
  }
}

void output_file(FILE* file, options* Options) {
  char previous_symbol = '\n';
  char current_symbol = getc(file);
  int str_number = 1;
  while (current_symbol != EOF) {
    if ((*Options).s) suppression(file, previous_symbol, current_symbol);

    if ((*Options).b)
      str_number =
          number_lines_notempty(previous_symbol, current_symbol, str_number);
    else if ((*Options).n)
      str_number = number_lines_all(previous_symbol, str_number);

    if ((*Options).e) {
      endline_dollar(previous_symbol, current_symbol, Options);
    }

    print_symbols(current_symbol, Options);

    previous_symbol = current_symbol;
    current_symbol = getc(file);
  }
}
void work_with_files(int files_count, char* files_path[], options* Options) {
  while (files_count > 0) {
    FILE* fp = fopen(*files_path, "r");
    if (fp == NULL) {
      output_err_file(*files_path);
    } else {
      output_file(fp, Options);
    }
    files_count--;
    files_path++;
    if (fp != NULL) fclose(fp);
  }
}

void read_options(int argc, char* argv[], options* Options) {
  int option_index = 0;

  char current_option = getopt_long(argc, argv, ALLOWED_OPTIONS_SHORT,
                                    ALLOWED_OPTIONS_LONG, &option_index);

  while (current_option != -1) {
    set_options(current_option, Options);
    current_option = getopt_long(argc, argv, ALLOWED_OPTIONS_SHORT,
                                 ALLOWED_OPTIONS_LONG, &option_index);
  }
}

void set_options(char c, options* Options) {
  if (c == 'A') {
    Options->v = 1;
    Options->e = 1;
    Options->t = 1;
  } else if (c == 'b') {
    Options->b = 1;
  } else if (c == 'e') {
    Options->v = 1;
    Options->e = 1;
  } else if (c == 'n') {
    Options->n = 1;
  } else if (c == 's') {
    Options->s = 1;
  } else if (c == 't') {
    Options->t = 1;
    Options->v = 1;
  } else if (c == 'E') {
    Options->e = 1;
  } else if (c == 'T') {
    Options->t = 1;
  } else if (c == 'u') {
    Options->u = 1;
  } else if (c == 'v') {
    Options->v = 1;
  } else if (c == '?') {
    exit(1);
  }
}

void output_err_file(char* files_path) {
  fprintf(stderr, "cat: %s: No such file or directory\n", files_path);
}
