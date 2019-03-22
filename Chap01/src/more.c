#include <stdio.h>
#include <stdlib.h>

int num_of_lines = 0;
const int LEN_OF_LINE = 512;

void
init_more();

void
do_more(FILE *);

int
see_more();

int
main(int argc, char *argv[], char *env[]) {
  init_more();
  if (argc == 1) {
    do_more(stdin);
  } else {
    while (--argc) {
      FILE *fp;
      if ((fp = fopen(*++argv, "r")) != NULL) {
        do_more(fp);
      } else {
        exit(1);
      }
    }
  }
  return 0;
}

void
init_more() {
  char * lines_str = getenv("LINES");
  printf("%s\n", lines_str);
  if (lines_str != NULL) {
    num_of_lines = atoi(lines_str) - 1;
  } else {
    num_of_lines = 24;
  }
  printf("num_of_lines: %d\n", num_of_lines);
}

void
do_more(FILE *fp) {
  int lines_left = num_of_lines;
  char line[LEN_OF_LINE];
  
  while ((fgets(line, LEN_OF_LINE, fp)) != NULL) {
    if (lines_left == 0) {
      lines_left += see_more();
      if (lines_left == 0) {
        break;
      }
    }
    if (fputs(line, stdout) == EOF) {
      exit(1);
    }
    --lines_left;
  }
}

int
see_more() {
  fputs("\033[7m more? \033[m", stdout);
  char c = getchar();
  switch (c) {
    case ' ':
      return num_of_lines;
    case '\n':
      return 1;
    default:
      return 0;
  }
}

