#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>

paddr_t isa_mmu_translate(vaddr_t vaddr, int type, int len) {
  paddr_t pg_table = paddr_read(cpu.cr3 + (vaddr >> 22) * sizeof(paddr_t), sizeof(paddr_t));
  if ((pg_table & 1) == 0) {
    printf("pg_table: 0x%08x, vaddr: 0x%08x, type: %d, len: %d\n", pg_table, vaddr, type, len);
  }
  assert(pg_table & 1);
  pg_table = pg_table & ADDRMASK;
  paddr_t pg_base = paddr_read(pg_table + ((vaddr >> 12) & 0x3ff) * sizeof(paddr_t), sizeof(paddr_t)) & ADDRMASK;
  // if (vaddr >= 0x40000000)
  //   printf("Translating 0x%08x to 0x%08x\n", vaddr, pg_base);
  if ((vaddr & FLAGMASK) + len - 1 > FLAGMASK)
    return pg_base | MEM_RET_CROSS_PAGE;
  return pg_base | MEM_RET_OK;
}
