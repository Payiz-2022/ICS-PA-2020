#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>

paddr_t isa_mmu_translate(vaddr_t vaddr, int type, int len) {
  paddr_t pg_table = paddr_read(cpu.cr3 + (vaddr >> 22) * sizeof(paddr_t), sizeof(paddr_t)) & ADDRMASK;
  paddr_t paddr = paddr_read(pg_table + ((vaddr >> 12) & 0x3ff) * sizeof(paddr_t), sizeof(paddr_t));
  paddr_t pg_base = paddr & ADDRMASK;
  // if (vaddr >= 0x40000000)
  //   printf("Translating 0x%08x to 0x%08x\n", vaddr, pg_base);
  if (pg_base != ((paddr + len - 1) & ADDRMASK))
    return pg_base | MEM_RET_CROSS_PAGE;
  return pg_base | MEM_RET_OK;
}
