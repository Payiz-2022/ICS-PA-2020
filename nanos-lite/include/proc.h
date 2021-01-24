#ifndef __PROC_H__
#define __PROC_H__

#include <common.h>
#include <memory.h>

#define STACK_SIZE (8 * PGSIZE)
#define MAX_NR_PROC 4
#define ADDRMASK 0xfffff000
#define FLAGMASK 0x00000fff

typedef union {
  uint8_t stack[STACK_SIZE] PG_ALIGN;
  struct {
    Context *cp;
    AddrSpace as;
    // we do not free memory, so use `max_brk' to determine when to call _map()
    uintptr_t max_brk;
  };
} PCB;

extern PCB *current;

void naive_uload(PCB*, const char*);
void context_kload(PCB*, const void*, void*);
void context_uload(PCB*, const char*, char*const*, char*const*);
Context* schedule(Context*);
void switch_boot_pcb();
void set_pcb(int);
PCB* get_free_pcb();
PCB* get_current_pcb();
PCB* get_last_pcb();

#endif
