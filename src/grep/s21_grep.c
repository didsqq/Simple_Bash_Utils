#include "s21_grep.h"

int main(int argc, char* argv[]) {
  options Options = {0};
  Options.patterns.size = 0;
  read_options(argc, argv, &Options);
  work_with_files(argc - optind, argv + optind, &Options);
  for (int i = 0; i < Options.patterns.size; i++) {
    regfree(&Options.patterns.reg_data[i]);
  }
  free(Options.patterns.reg_data);
  return 0;
}

char* read_str(FILE* fp, char* buffer) {
  int i = 0;
  char ch = getc(fp);
  while (ch != '\n' && ch != EOF) {
    buffer[i++] = ch;
    ch = getc(fp);
  }
  buffer[i] = '\0';
  return buffer;
}

void work_with_file(FILE* fp, options* Options, int files_count,
                    char* file_path) {
  static int all_files_count = 0;
  all_files_count += files_count;
  int flag = 0, counter_coincidence = 0, counter_line = 0;
  regmatch_t match = {0};
  while (flag != 1) {
    char ch = getc(fp);
    if (ch != EOF) {
      ungetc(ch, fp);
      char* buffer = safe_malloc(sizeof(char) * 4096);
      buffer = read_str(fp, buffer);
      counter_line++;
      int trigger_v = 0;  // переменная для флага v было ли совпадение
      char* line_ptr = buffer;
      for (int i = 0; i < Options->patterns.size; i++) {
        int reti =
            regexec(&Options->patterns.reg_data[i], buffer, 1, &match, 0);
        if (Options->l && !reti && !Options->v && !Options->c) {  //опция l
          printf("%s\n", file_path);
          flag = 1;
          i = Options->patterns.size;
        }
        if (!reti) {
          name_number(Options, all_files_count, file_path, counter_line);
          flag += opt_o(Options, line_ptr, match, file_path, all_files_count,
                        counter_line, buffer, i);  //опция о
        }
        if (!reti && !Options->c && !Options->l && !Options->v &&
            !Options->o)  //просто вывод строки
          printf("%s\n", buffer);
        if (!reti) {
          counter_coincidence++;
          trigger_v = 1;
          i = Options->patterns.size;
        }
      }
      flag += opt_v(Options, file_path, trigger_v, all_files_count,
                    counter_line, buffer);
      if (buffer != NULL) free(buffer);
    } else {
      flag = 1;
      opt_c(Options, all_files_count, counter_coincidence, counter_line,
            file_path);
    }
  }
}

void name_number(options* Options, int all_files_count, char* file_path,
                 int counter_line) {
  if (all_files_count != 1 && !Options->h && !Options->v && !Options->c &&
      !Options->l)  //название файла если файл не 1
    printf("%s:", file_path);

  if (Options->n && !Options->c && !Options->v && !Options->l)  //опция n
    printf("%d:", counter_line);
}

int opt_o(options* Options, char* line_ptr, regmatch_t match, char* file_path,
          int all_files_count, int counter_line, char* buffer, int i) {
  int flag = 0;
  int reti;
  if (Options->o && !Options->c && !Options->v && !Options->l) {
    printf("%.*s\n", (int)(match.rm_eo - match.rm_so),
           (line_ptr + match.rm_so));
    line_ptr += match.rm_eo;
    while (!(reti = regexec(&Options->patterns.reg_data[i], line_ptr, 1, &match,
                            0))) {
      printf("%.*s\n", (int)(match.rm_eo - match.rm_so),
             (line_ptr + match.rm_so));
      line_ptr += match.rm_eo;
      flag +=
          opt_v(Options, file_path, 1, all_files_count, counter_line, buffer);
    }
  }
  return flag;
}

int opt_v(options* Options, char* file_path, int trigger_v, int all_files_count,
          int counter_line, char* buffer) {
  int flag = 0;
  if (Options->l && !trigger_v && Options->v && !Options->c) {
    printf("%s\n", file_path);
    flag = 1;
  } else if (Options->v && !trigger_v && all_files_count != 1 && !Options->h &&
             !Options->c)
    printf("%s:", file_path);
  if (Options->v && !trigger_v && Options->n && !Options->c && !Options->l)
    printf("%d:%s\n", counter_line, buffer);
  else if (Options->v && !trigger_v && !Options->c && !Options->l)
    printf("%s\n", buffer);

  return flag;
}

void opt_c(options* Options, int all_files_count, int counter_coincidence,
           int counter_line, char* file_path) {
  if (Options->c && all_files_count != 1 && !Options->h)
    printf("%s:", file_path);
  if (Options->c && Options->v && !Options->l)
    printf("%d\n", counter_line - counter_coincidence);
  else if (Options->c && Options->l && counter_coincidence != 0)
    printf("%d\n%s\n", 1, file_path);
  else if (Options->c)
    printf("%d\n", counter_coincidence);
}

void work_with_files(int files_count, char* files_path[], options* Options) {
  if (!Options->f && !Options->e) {
    init_patterns(&Options->patterns);
    patterns_add_console(Options, *files_path);
    files_count--;
    files_path++;
  }
  int all_files_count = files_count;
  while (files_count > 0) {
    FILE* fp = safe_fopen(*files_path, "r", Options);
    if (fp != NULL && Options->patterns.size != 0)
      work_with_file(fp, Options, all_files_count, *files_path);
    files_count--;
    files_path++;
    if (fp != NULL) fclose(fp);
  }
}

void patterns_add_console(options* Options, char* optarg) {
  int reti = 0;
  if (Options->i)
    reti = regcomp(&Options->patterns.reg_data[Options->patterns.size - 1],
                   optarg, REG_ICASE);
  else
    reti = regcomp(&Options->patterns.reg_data[Options->patterns.size - 1],
                   optarg, 0);
  if (reti) {
    fprintf(stderr, "Could not compile regex\n");
    exit(1);
  }
}

void patterns_add_file(options* Options, char* optarg) {
  FILE* fp = safe_fopen(optarg, "r", Options);
  int flag = 0;
  if (fp == NULL) flag = 1;
  while (flag != 1) {
    char ch = getc(fp);
    if (ch != EOF) {
      ungetc(ch, fp);
      char* buffer = safe_malloc(sizeof(char) * 4096);
      buffer = read_str(fp, buffer);
      int reti = 0;
      if (buffer != NULL && Options->i) {
        init_patterns(&Options->patterns);
        reti = regcomp(&Options->patterns.reg_data[Options->patterns.size - 1],
                       buffer, REG_ICASE);
      } else if (buffer != NULL) {
        init_patterns(&Options->patterns);
        reti = regcomp(&Options->patterns.reg_data[Options->patterns.size - 1],
                       buffer, 0);
      }
      if (buffer != NULL) free(buffer);
      if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
      }
    } else {
      flag = 1;
    }
  }
  if (fp != NULL) {
    fclose(fp);
  } else if (Options->s) {
    fprintf(stderr, "grep: %s: No such file or directory\n", optarg);
  }
}

void set_options(char c, options* Options) {
  if (c == 'e') {
    Options->e = 1;
    init_patterns(&Options->patterns);
    patterns_add_console(Options, optarg);
  } else if (c == 'f') {
    Options->f = 1;
    patterns_add_file(Options, optarg);
  } else if (c == 'i') {
    Options->i = 1;
  } else if (c == 'v') {
    Options->v = 1;
  } else if (c == 'c') {
    Options->c = 1;
  } else if (c == 'l') {
    Options->l = 1;
  } else if (c == 'n') {
    Options->n = 1;
  } else if (c == 'h') {
    Options->h = 1;
  } else if (c == 's') {
    Options->s = 1;
  } else if (c == 'o') {
    Options->o = 1;
  }
}

void init_patterns(Patterns* patterns) {
  if (patterns->size == 0) {
    patterns->size++;
    patterns->reg_data = safe_malloc(sizeof(regex_t));
  } else {
    patterns->size++;
    patterns->reg_data =
        safe_realloc(patterns->reg_data, sizeof(regex_t) * patterns->size);
  }
}

void read_options(int argc, char* argv[], options* Options) {
  int option_index = 0;
  char current_option = getopt_long(argc, argv, SHORTOPTS, 0, &option_index);
  while (current_option != -1) {
    set_options(current_option, Options);
    current_option = getopt_long(argc, argv, SHORTOPTS, 0, &option_index);
  }
}

void* safe_malloc(size_t size) {
  void* ret_ptr = malloc(size);
  if (ret_ptr == NULL) {
    fprintf(stderr, "%zu: Memory allocation error.\n", size);
    exit(1);
  }
  return ret_ptr;
}

void* safe_realloc(void* ptr, size_t size) {
  void* ret_ptr = realloc(ptr, size);
  if (ret_ptr == NULL) {
    fprintf(stderr, "%zu: Memory reallocation error.\n", size);
    exit(1);
  }
  return ret_ptr;
}

FILE* safe_fopen(const char* filename, char* modes, options* Options) {
  FILE* ret_ptr = fopen(filename, modes);
  if (ret_ptr == NULL && !Options->s) {
    fprintf(stderr, "grep: %s: No such file or directory\n", filename);
  }
  return ret_ptr;
}