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
  char* args[] = {"/bin/pal", "--skip", NULL};
  context_uload(&pcb[1], "/bin/pal", args, NULL);
  args[0] = "/bin/hello";
  context_uload(&pcb[2], "/bin/hello", args, NULL);
  switch_boot_pcb();
}

int pcb_id = 1;
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

Context* schedule(Context *prev, int force) {
  if (!force && current == &pcb_boot) return prev;
  current->cp = prev;
  #ifdef DEBUG
    Log("[Schedule] Saved current context pointer to 0x%08x", prev);
  #endif
  current = (current == &pcb[1] ? &pcb[1] : &pcb[1]);
  #ifdef DEBUG
    Log("[Schedule] Switched to 0x%08x, context 0x%08x, entry 0x%08x", current, current->cp, current->cp->eip);
  #endif
  return current->cp;
}
