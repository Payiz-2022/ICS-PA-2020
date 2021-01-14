#include <sys/time.h>
#include <stdio.h>

int main() {
  struct timeval current_time;
  gettimeofday(&current_time, NULL);
  while (1) {
    // if (current_time.tv_usec % 1000 == 0)
      printf("usec: %ld\n", current_time.tv_usec);
  }
  return 0;
}
