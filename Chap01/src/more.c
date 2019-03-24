#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>

int num_of_lines = 0;
struct termios old;
FILE *cmd;
int CMD_FILENO;

const int LEN_OF_LINE = 512;
const char * const CMD_FILE = "/dev/tty";

void
init_more();

void
exit_more();

void
do_more(FILE *);

int
see_more(long cur_sz, long file_sz);

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
 * close cmd file
 * show the cursor
 */
void
exit_more() {
  tcsetattr(CMD_FILENO, TCSANOW, &old);
  fclose(cmd);
  fputs("\033[?25h", stdout);
}

/*
 * call exit_more at exit
 * get number of lines of the terminal
 * set the terminal to not echo keypress and send it immediately
 * open /dev/tty as the input of commands
 * hide the cursor
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

  // open /dev/tty
  if ((cmd = fopen(CMD_FILE, "r")) == NULL) {
    exit(1);
  }
  CMD_FILENO = fileno(cmd);

  // set the terminal mode
  struct termios tm;
  tcgetattr(CMD_FILENO, &old);
  tm = old;
  tm.c_lflag &= ~(ICANON | ECHO);
  tm.c_cc[VMIN] = 1;
  tm.c_cc[VTIME] = 0;
  if (tcsetattr(CMD_FILENO, TCSADRAIN, &tm) == -1) {
    perror("tcsetattr");
  }

  // hide the cursor
  fputs("\033[?25l", stdout);
}

/*
 * get file size
 * print num_of_lines lines of file
 * then call see_more() to get command
 */
void
do_more(FILE *fp) {
  // get file size
  long file_sz;
  if (fp != stdin) {
    if  (fseek(fp, 0, SEEK_END) == -1) {
      exit(1);
    }
    file_sz = ftell(fp);
    if  (fseek(fp, 0, SEEK_SET) == -1) {
      exit(1);
    }
  } else {  // stdin, no persentage 
    file_sz = -1;
  }

  // print and get command 
  int lines_left = num_of_lines;
  char line[LEN_OF_LINE];
  long cur_sz = 0;
  
  while ((fgets(line, LEN_OF_LINE, fp)) != NULL) {
    if (file_sz > 0) { // not stdin, percentage needed
      cur_sz += strlen(line);
    }
    if (lines_left == 0) {
      lines_left += see_more(cur_sz, file_sz);
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
see_more(long cur_sz, long file_sz) {
  char c;
  char per[10]; // percentage string

  // get percentage string
  if (file_sz > 0) {
    sprintf(per, "(%d%%)", (int) (cur_sz / (double) file_sz * 100));
  }

  // invert color and print more and percentage
  printf("\033[7m More?%s \033[m", (file_sz > 0) ? per : "");

  // put cursor back to the beginning of the line
  printf("\033[%dD", num_of_lines);

  while ((c = fgetc(cmd)) != EOF) {
    switch (c) {
      case ' ':
        // erase the line
        fputs("\033[0K", stdout);
        return num_of_lines;
      case '\n':
        // erase the line
        fputs("\033[0K", stdout);
        return 1;
      case 'q':
        // erase the line
        fputs("\033[0K", stdout);
        return 0;
    }
  }
  return 0;
}

