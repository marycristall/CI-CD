#ifndef S21_GREP_H_
#define S21_GREP_H_

#define BUFFER_SIZE 5000

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct opts {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
};

void opts_parser(int argc, char **argv, struct opts *opts, char *patternE);
void processing(struct opts *opts, FILE *fp, regex_t reg, char *file);
void grep_file(struct opts *opts, char *pattern, char *file);
void grep(struct opts *opts, int argc, char **argv, char *temp);

#endif
