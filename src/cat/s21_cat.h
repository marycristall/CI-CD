#ifndef S21_CAT_H
#define S21_CAT_H
#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define OPTIONS_END -1
#define ASCII_DEL 127
#define NONPRINTING_SHIFT 64

static const char SHORT_OPTIONS[] = "beEnstT";

static const struct option LONG_OPTIONS[] = {{"number-nonblank", 0, NULL, 'b'},
                                             {"number", 0, NULL, 'n'},
                                             {"squeeze-blank", 0, NULL, 's'},
                                             {NULL, 0, NULL, 0}};
typedef struct Options {
  bool b;
  bool e;
  bool n;
  bool s;
  bool t;
  bool v;
} Options;

static void opt_set(const char option, Options *const options);
static void print_errors(void);
static void init_options(int argc, char *const argv[], Options *options);
static void print_file(FILE *file, const Options *const options);
static void process_files(int file_count, char *const file_path[],
                          const Options *const options);
static void print_file(FILE *file, const Options *const options);
static void end_line(char current_symbol, const Options *const options);
static void number_line(char previous_symbol, char current_symbol,
                        const Options *const options);
static void simple_print(const char current_symbol);
static void print_cntrl(const char current_symbol,
                        const Options *const options);
static void print_tab(const char current_symbol, const Options *const options);
static void print_meta(const char current_symbol, const Options *const options);
static void print_symbol(const char current_symbol,
                         const Options *const options);

#endif
