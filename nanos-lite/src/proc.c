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
    printf("bef: 0x02a4efe4: 0x%08x\n", *(uint32_t*)0x02a4efe4);
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    printf("aft: 0x02a4efe4: 0x%08x\n", *(uint32_t*)0x02a4efe4);
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
  context_kload(&pcb[2], hello_fun, (void*)0xd);
  switch_boot_pcb();
}

Context* schedule(Context *prev) {
  current->cp = prev;
  printf("Saved current cp: 0x%08x\n", current->cp);
  current = (current == &pcb[0] ? &pcb[2] : &pcb[0]);
  printf("Current cp: 0x%08x\n", current->cp);
  return current->cp;
}
