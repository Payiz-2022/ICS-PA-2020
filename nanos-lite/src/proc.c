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

  context_kload(&pcb[0], hello_fun, (void*)0xeeeee);
  char* args[] = {"/bin/nterm", NULL};
  context_uload(&pcb[1], "/bin/nterm", args, NULL);
  switch_boot_pcb();
}

int pcb_id = 1;
PCB* get_free_pcb() {
  pcb_id++;
  printf("Goto PCB: %d (0x%08x) cp: 0x%08x\n", pcb_id, &pcb[pcb_id], pcb[pcb_id].cp);
  if (pcb_id == MAX_NR_PROC) panic("No free PCB available");
  return &pcb[pcb_id];
}

PCB* get_current_pcb() {
  return &pcb[pcb_id];
}

PCB* get_last_pcb() {
  pcb_id--;
  if (pcb_id < 0) pcb_id = 0;
  #ifdef DEBUG
    printf("Goto PCB: %d (0x%08x) cp: 0x%08x\n", pcb_id, &pcb[pcb_id], pcb[pcb_id].cp);
  #endif
  return &pcb[pcb_id];
}

Context* schedule(Context *prev) {
  current->cp = prev;
  #ifdef DEBUG
    printf("Saved current context pointer to 0x%08x\n", prev);
  #endif
  current = (current == &pcb[0] ? &pcb[pcb_id] : &pcb[0]);
  return current->cp;
}
