#include <NDL.h>
#include <sys/time.h>
#include <stdio.h>

int main() {
  // struct timeval current_time;
  // gettimeofday(&current_time, NULL);
  // long int last_sec = -1;
  // while (1) {
  //   gettimeofday(&current_time, NULL);
  //   if (current_time.tv_sec != last_sec) {
  //     last_sec = current_time.tv_sec;
  //     printf("Second: %ld Usec: %ld\n", current_time.tv_sec, current_time.tv_usec);
  //   }
  // }
  uint32_t current_msec, last_sec = -1;
  while (1) {
    current_msec = NDL_GetTicks();
    if (current_msec / 1000 != last_sec) {
      last_sec = current_msec / 1000;
      printf("MilliSecond: %d\n", last_sec);
    }
  }
  return 0;
}
