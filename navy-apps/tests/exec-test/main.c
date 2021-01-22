#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("Running with argc %d argv %s\n", argc, argc == 2 ? argv[1] : "NULL");
  int n = (argc >= 2 ? atoi(argv[1]) : 1);
  printf("%s: argv[1] = %d\n", argv[0], n);

  char buf[16];
  sprintf(buf, "%d", n + 1);
  execl(argv[0], argv[0], buf, NULL);
  return 0;
}
