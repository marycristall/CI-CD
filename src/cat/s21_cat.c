#include "s21_cat.h"
int main(int argc, char *const argv[]) {
  Options options = {0};
  init_options(argc, argv, &options);
  process_files(argc - optind, argv + optind, &options);
  return EXIT_SUCCESS;
}

static void opt_set(const char option, Options *options) {
  switch (option) {
    case 'b':  //нумерует непустые строки
      options->b = true;
      break;
    case 'e':  //отображает символы конца строки как $ + -v
      options->e = true;
      options->v = true;
      break;
    case 'E':  //символы конца строки как $ без -v
      options->e = true;
      break;
    case 'n':  // нумерует все строки
      options->n = true;
      break;
    case 's':  //сжимает несколько соседних пустых строк
      options->s = true;
      break;
    case 't':  //отображает табы (и -v
      options->t = true;
      options->v = true;
      break;
    case 'T':
      options->t = true;
      break;
    default:
      print_errors();
  }
}
static void print_errors(void) {  //печать ошибок
  fprintf(stderr, "try again, invalid flag or file doesn't exist.\n");
  exit(EXIT_FAILURE);
}

static void init_options(
    int argc, char *const argv[],
    Options *options) {  //считывает опцию и обрабатывает case
  int long_options_index = 0;
  char current_option =
      getopt_long(argc, argv, SHORT_OPTIONS, LONG_OPTIONS, &long_options_index);
  while (current_option != OPTIONS_END) {
    opt_set(current_option, options);
    current_option = getopt_long(argc, argv, SHORT_OPTIONS, LONG_OPTIONS,
                                 &long_options_index);
  }
}
static void number_line(char previous_symbol, char current_symbol,
                        const Options *const options) {
  //обработка -b,-n
  static unsigned line_count = 0;
  if (options->b && previous_symbol == '\n' && current_symbol != '\n') {
    fprintf(stdout, "%6u\t", ++line_count);
  } else if (options->n && previous_symbol == '\n') {
    fprintf(stdout, "%6u\t", ++line_count);
  }
}
static void end_line(char current_symbol,
                     const Options *const options) {  //обработка -е
  if (options->e && current_symbol == '\n') {
    fputc('$', stdout);
  }
}
static void squeeze_blank(FILE *file) {  //обработка -s
  char current_symbol = fgetc(file);
  while (current_symbol == '\n') {
    current_symbol = fgetc(file);
  }
  ungetc(current_symbol, file);
}
static void print_symbol(const char current_symbol,
                         const Options *const options) {
  if (isprint(current_symbol)) {  //возвращает не 0, если символ печатаемый
    simple_print(current_symbol);
  } else if (current_symbol == '\t') {
    print_tab(current_symbol, options);
  } else if (current_symbol == '\n') {
    printf("\n");
  } else if (iscntrl(
                 current_symbol)) {  //возвращает не 0, если символ управляющий
    print_cntrl(current_symbol, options);
  } else {
    print_meta(current_symbol, options);
  }
}

static void print_cntrl(
    const char current_symbol,
    const Options *const options) {  //печать управляющих символов
  if (options->v) {
    simple_print('^');
    if (current_symbol == ASCII_DEL) {
      simple_print(current_symbol - NONPRINTING_SHIFT);
    } else {
      simple_print(current_symbol + NONPRINTING_SHIFT);
    }
  } else {
    simple_print(current_symbol);
  }
}

static void print_tab(const char current_symbol, const Options *const options) {
  if (options->t) {
    simple_print('^');
    simple_print(current_symbol + NONPRINTING_SHIFT);
  } else {
    simple_print(current_symbol);
  }
}

static void simple_print(
    const char current_symbol) {  //печать символа в поток вывода
  fputc(current_symbol, stdout);
}

static void print_meta(const char current_symbol,
                       const Options *const options) {
  if (options->v) {
    const char meta_symbol = (signed char)current_symbol + ASCII_DEL + 1;
    simple_print('M');
    simple_print('-');
    if (isprint(meta_symbol)) {
      simple_print(meta_symbol);
    } else {
      print_cntrl(meta_symbol, options);
    }
  } else {
    simple_print(current_symbol);
  }
}

static void print_file(FILE *file, const Options *const options) {
  char previous_symbol = '\n';
  char current_symbol = fgetc(file);
  while (current_symbol != EOF) {
    number_line(previous_symbol, current_symbol, options);
    end_line(current_symbol, options);
    print_symbol(current_symbol, options);
    if (options->s && previous_symbol == '\n' && current_symbol == '\n') {
      squeeze_blank(file);
    }
    previous_symbol = current_symbol;
    current_symbol = fgetc(file);
  }
}
static void process_files(int file_count, char *const file_path[],
                          const Options *const options) {
  FILE *current_file = NULL;
  while (file_count > 0) {
    current_file = fopen(*file_path, "r");
    if (current_file == NULL) {
      print_errors();
    } else {
      print_file(current_file, options);
    }
    ++file_path;
    --file_count;
  }
}
