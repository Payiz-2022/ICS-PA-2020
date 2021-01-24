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
  char* args[] = {"/bin/pal", NULL};
  context_uload(&pcb[0], "/bin/pal", args, NULL);
  switch_boot_pcb();

  printf("40038e6a: 0x%08x\n", *(uint32_t*)0x40038e6a);
}

int pcb_id = 0;
PCB* get_free_pcb() {
  pcb_id++;
  #ifdef DEBUG
    Log("Goto PCB: %d (0x%08x) cp: 0x%08x", pcb_id, &pcb[pcb_id], pcb[pcb_id].cp);
  #endif
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
    Log("Goto PCB: %d (0x%08x) cp: 0x%08x", pcb_id, &pcb[pcb_id], pcb[pcb_id].cp);
  #endif
  return &pcb[pcb_id];
}

Context* schedule(Context *prev) {
  current->cp = prev;
  #ifdef DEBUG
    Log("[Schedule] Saved current context pointer to 0x%08x", prev);
  #endif
  current = (current == &pcb[0] ? &pcb[pcb_id] : &pcb[0]);
  return current->cp;
}
