#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  char *empty[] =  {NULL };
  environ = empty;
  printf("args addr: 0x%08x", args);
  exit(main(*args, (char**)(args + 1), empty));
  assert(0);
}
