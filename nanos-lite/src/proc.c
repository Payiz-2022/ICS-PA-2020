#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
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

  context_kload(&pcb[0], hello_fun, (void*)0xf);
  context_kload(&pcb[1], hello_fun, (void*)0xd);
  switch_boot_pcb();
}

bool which = 0;
Context* schedule(Context *prev) {
  current->cp = prev;
  current = &pcb[which];
  which = !which;
  return current->cp;
}
