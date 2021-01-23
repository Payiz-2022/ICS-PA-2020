#include <memory.h>

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

static inline void* pg_alloc(int n) {
  void* ret = new_page(n / PGSIZE);
  memset(ret, 0, n);
  return ret;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
