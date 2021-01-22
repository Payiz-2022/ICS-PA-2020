#include <proc.h>

static PCB pcb_boot = {};
static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
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
  char* args[] = {"/bin/nterm", NULL};
  context_uload(&pcb[0], "/bin/nterm", args, NULL);
  switch_boot_pcb();
}

int pcb_id = 0;
PCB* get_free_pcb() {
  pcb_id = (pcb_id + 1) % MAX_NR_PROC;
  return &pcb[pcb_id];
  for (int i = 0; i < MAX_NR_PROC; i++)
    if (pcb[i].cp == NULL)
      return &pcb[i];
  panic("No free PCB available");
  return NULL;
}

PCB* get_last_pcb() {
  pcb_id = (pcb_id - 1) % MAX_NR_PROC;
  return &pcb[pcb_id];
}

Context* schedule(Context *prev) {
  current->cp = prev;
  current = &pcb[pcb_id];
  return current->cp;
}
