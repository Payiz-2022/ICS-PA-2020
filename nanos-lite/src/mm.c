#include <memory.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void*)heap.end);
  #ifdef DEBUG
    Log("[MM] New page allocated at 0x%08x", p);
  #endif
  return p;
}

void* pg_alloc(int n) {
  assert(n % PGSIZE == 0);
  void* ret = new_page(n / PGSIZE);
  memset(ret, 0, n);
  return ret;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
#ifdef HAS_VME
  PCB* pcb = get_current_pcb();
  #ifdef DEBUG
    Log("[MM] Memory brk increased from 0x%08x to 0x%08x", pcb->max_brk, brk);
  #endif

  if (pcb->max_brk && pcb->max_brk < brk) {
    uintptr_t begin = pcb->max_brk >> 12;
    uintptr_t end = brk >> 12;
    for (uintptr_t i = begin + 1; i <= end; i++) {
      map(&pcb->as, (void*)(i << 12), pg_alloc(PGSIZE), 0);
    }
  }
  pcb->max_brk = max(pcb->max_brk, brk);
#endif
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
