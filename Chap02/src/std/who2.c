/* who1.c - a first version of the who program
 *           open, read UTMP file, and show results
 */
#include <stdio.h>
#include <stdlib.h>
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

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

void
show_time(time_t t);

/*
 * show info()
 * displays contents of the utmp struct in human readable form
 * * note * these sizes should not be hardwired
 */
void
show_info(struct utmp *record) {
  if (record->ut_type != USER_PROCESS) { // users only
    return;
  }
  printf("%-8.8s", record->ut_name);
  printf(" ");
  printf("%-8.8s", record->ut_line);
  printf(" ");
  show_time(record->ut_time);
  printf(" ");
#ifdef SHOWHOST
  printf("(%-8.8s)", record->ut_host);
#endif
  printf("\n");
}

/*
 * displays time in a format fit for human consumption
 * uses ctime to build a string then picks parts out of it
 * Note: %12.12s prints a string 12 chars wid and LIMITS
 * it to 12 chars
 */
void
show_time(time_t t) {
  char *cp = ctime(&t);    /* hold address to time
                              convert time to string
                              string looks like 
                              Mon Feb 4 00:46:40 1991
                              0123456789012345 */
  printf("%12.12s", cp + 4);
}
