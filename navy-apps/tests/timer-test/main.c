#include <sys/time.h>
#include <stdio.h>

int main() {
  struct timeval current_time;
  gettimeofday(&current_time, NULL);
  while (1) {
    gettimeofday(&current_time, NULL);
    if (current_time.tv_usec % 1000000 == 0)
      printf("Second: %ld\n", current_time.tv_sec);
  }
  return 0;
}
