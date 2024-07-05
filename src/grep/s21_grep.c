#include "s21_grep.h"

int main(int argc, char *argv[]) {
  struct opts opts = {0};
  char patternE[BUFFER_SIZE] = {0};
  opts_parser(argc, argv, &opts, patternE);
  if (argc >= 3) grep(&opts, argc, argv, patternE);
  return 0;
}

void opts_parser(int argc, char **argv, struct opts *opts,
                 char *patternE) {  // считывание и запоминание опций
  int opt = 0;
  const char *s_options = "e:ivcln";
  while ((opt = getopt(argc, argv, s_options)) != -1) {
    switch (opt) {
      case 'e':  // шаблон
        opts->e = 1;
        snprintf(patternE, BUFFER_SIZE, "%s", optarg);
        break;
      case 'i':  // поиск с игнорированием регистра
        opts->i = 1;
        break;
      case 'v':  // инверсия поиска
        opts->v = 1;
        break;
      case 'c':  // выводит количество строк с совпадениями
        opts->c = 1;
        break;
      case 'l':  // выводит название файла, где есть совпадение
        opts->l = 1;
        break;
      case 'n':  // показывает номера строк с совпадениями
        opts->n = 1;
        break;
      default:
        printf("error\n");
        break;
    }
  }
}

void processing(struct opts *opts, FILE *fp, regex_t reg,
                char *file) {  // обработка файла
  char text[BUFFER_SIZE] = {0};
  regmatch_t pmatch[1];
  int line_matches = 0, nline = 1;
  while (fgets(text, BUFFER_SIZE - 1, fp) != NULL) {
    int match = 0;
    int success = regexec(&reg, text, 1, pmatch, 0);  // ищем слово
    if (strchr(text, '\n') == NULL) strcat(text, "\n");
    if (success == 0 && !opts->v) match = 1;
    if (success == REG_NOMATCH && opts->v) match = 1;
    if (match && !opts->l && !opts->c && opts->n) printf("%d:", nline);
    if (match && !opts->l && !opts->c) {
      printf("%s", text);
    }
    line_matches += match;
    nline++;
  }
  if (opts->l && line_matches > 0) printf("%s\n", file);
  if (opts->c && !opts->l) printf("%d\n", line_matches);
}

void grep_file(struct opts *opts, char *pattern, char *filename) {
  int copts = REG_EXTENDED;  // используется синтаксис расширенных регулярных
                             // рыражений POSIX во время интерпретации regex
  regex_t reg;
  FILE *file;
  file = fopen(filename, "r");
  if (opts->i) copts = REG_ICASE;  // игнорируется регистр
  if (file != NULL) {
    regcomp(&reg, pattern, copts);
    processing(opts, file, reg, filename);
    regfree(&reg);
    fclose(file);
  }
}

void grep(struct opts *opts, int argc, char **argv, char *buff) {
  char pattern[BUFFER_SIZE] = {0};
  int f_patt = 0;
  if (!opts->e)
    snprintf(pattern, BUFFER_SIZE, "%s",
             argv[optind++]);  // вывод данных в символьную строку str
  if (opts->e) snprintf(pattern, BUFFER_SIZE, "%s", buff);
  if (f_patt != -1) {
    int file_c = 0;
    if (argc - optind > 1) file_c = 1;
    for (int i = optind; i < argc; i++) {
      if (file_c && !opts->l) printf("%s:", argv[i]);
      grep_file(opts, pattern, argv[i]);
    }
  }
}
