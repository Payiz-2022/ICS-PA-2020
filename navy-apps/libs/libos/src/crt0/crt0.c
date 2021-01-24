#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  printf("Hello world\n");
  char *empty[] =  {NULL };
  environ = empty;
  printf("args addr: 0x%08x\n", args);
  exit(main(*args, (char**)(args + 1), empty));
  assert(0);
}
