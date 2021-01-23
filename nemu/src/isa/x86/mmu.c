#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>

paddr_t isa_mmu_translate(vaddr_t vaddr, int type, int len) {
  uintptr_t* pg_table = (uintptr_t*)((uintptr_t*)cpu.cr3)[vaddr >> 22];
  uintptr_t page = (vaddr >> 12) & 0x3ff;
  if ((vaddr & 0xfffff000) != ((vaddr + len) & 0xfffff000))
    return (pg_table[page] & 0xfffff000) | MEM_RET_CROSS_PAGE;
  return (pg_table[page] & 0xfffff000) | MEM_RET_OK;
}
