#include <proc.h>

static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  // switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  // naive_uload(NULL, "/bin/menu");

  // context_kload(&pcb[0], hello_fun, (void*)0xeeeee);
  char* args[] = {"/bin/exec-test", NULL};
  context_uload(&pcb[0], "/bin/exec-test", args, NULL);
  for (int i = 0; i < 4; i++)
    printf("#%d: 0x%08x ", i, &pcb[i]);
  switch_boot_pcb();
}

Context* schedule(Context *prev) {
  printf("schedule\n");
  current->cp = prev;
  current = &pcb[0];
  return current->cp;
}
