#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>

int num_of_lines = 0;
struct termios old;
FILE *cmd;

const int LEN_OF_LINE = 512;
const char *CMD_FILE = "/dev/tty";

void
init_more();

void
exit_more();

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
        fclose(fp);
      } else {
        exit(1);
      }
    }
  }
  return 0;
}

/*
 * set the terminal back to normal mode
 */
void
exit_more() {
  tcsetattr(STDIN_FILENO, TCSANOW, &old);
  fclose(cmd);
}

/*
 * call exit_more at exit
 * get number of lines of the terminal
 * set the terminal to not echo keypress and send it immediately
 * open /dev/tty as the input of commands
 */
void
init_more() {
  // call exit_more at exit
  atexit(exit_more);

  // get number of lines
  char * lines_str = getenv("LINES");
  struct winsize ws;
  if (lines_str != NULL) {
    num_of_lines = atoi(lines_str) - 1;
  } else if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0){
    num_of_lines = ws.ws_row;
  } else {
    num_of_lines = 24;
  }

  // set the terminal mode
  struct termios tm;
  tcgetattr(STDIN_FILENO, &old);
  tm = old;
  tm.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &tm);

  // open /dev/tty
  if ((cmd = fopen(CMD_FILE, "r")) == NULL) {
    exit(1);
  }
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
  char c;
  fputs("\033[7m more? \033[m", stdout);
  while ((c = fgetc(cmd)) != EOF) {
    switch (c) {
      case ' ':
        return num_of_lines;
      case '\n':
        return 1;
      case 'q':
        return 0;
    }
  }
  return 0;
}

