/*
 * @author: Rivers
 * version 0.1 of command `who`
 * write it myself to compare with standard version
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <utmp.h>

#define UTMP_PATH "/var/run/utmp"

void
show_info(const struct utmp *ut);

int main() {
  int utmp_no = open(UTMP_PATH, O_RDONLY);
  if (utmp_no == -1) {
    perror("open utmp");
    exit(1);
  }

  struct utmp ut;
  while (read(utmp_no, &ut, sizeof(struct utmp)) == sizeof(struct utmp)) {
    show_info(&ut);
  }

  if (close(utmp_no) == -1) {
    perror("close");
    exit(1);
  }

  return 0;
}

/*
 * show the info in ut
 */
void
show_info(const struct utmp *ut) {
  printf("%s\t%s\t%d\t", ut->ut_user, ut->ut_line, ut->ut_tv.tv_sec + ut->ut_tv.tv_usec / 1000);

  char proc_path[20];
  sprintf(proc_path, "/proc/%d/stat", ut->ut_pid);

  int proc_fno = open(proc_path, O_RDONLY);
  char buf[1024];
  int lparen, rparen;
  if (proc_fno == -1) {
    lparen = -1;
    rparen = 0;
  } else {
    if (read(proc_fno, buf, sizeof(buf)) == -1) {
      perror("read proc");
      exit(1);
    }

    close(proc_fno);

    lparen = strchr(buf, '(') - buf;
    rparen = strrchr(buf, ')') - buf;
  }
  buf[rparen] = '\0';
  printf("(%s)\n", buf + lparen + 1);
}
