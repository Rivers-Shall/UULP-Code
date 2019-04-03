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
  printf("(%s)", record->ut_host);
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
  struct tm *tmp = localtime(&t);    /* hold address to time
                              convert time to broken time
                              broken time has multiple fields like
                              year, month, day, hour, min, 
                              sec, etc.  See ctime(3) for more info */
  printf("%02d-%02d-%02d %02d:%02d", tmp->tm_year + 1900, 
                           tmp->tm_mon + 1, tmp->tm_mday,
                           tmp->tm_hour, tmp->tm_min);
}
