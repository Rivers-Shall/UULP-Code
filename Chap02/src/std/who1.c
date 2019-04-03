/* who1.c - a first version of the who program
 *           open, read UTMP file, and show results
 */
#include <stdio.h>
#include <stdlib.h>
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>

#define SHOWHOST                    /* include remote machine on output*/

void
show_info(struct utmp *record);

int main() {
  struct utmp current_record;       /* read info into here */
  int         utmpfd;               /* read from this descriptor */
  int         reclen = sizeof(struct utmp);

  if ((utmpfd = open(UTMP_FILE, O_RDONLY)) == -1) {
    perror(UTMP_FILE);
    exit(1);
  }

  while (read(utmpfd, &current_record, reclen) == reclen) {
    show_info(&current_record);
  }
  close(utmpfd);
  return 0;
}

/*
 * show info()
 * displays contents of the utmp struct in human readable form
 * * note * these sizes should not be hardwired
 */
void
show_info(struct utmp *record) {
  printf("%-8.8s", record->ut_name);
  printf(" ");
  printf("%-8.8s", record->ut_line);
  printf(" ");
  printf("%10d", record->ut_time);
  printf(" ");
#ifdef SHOWHOST
  printf("(%-8.8s)", record->ut_host);
#endif
  printf("\n");
}
