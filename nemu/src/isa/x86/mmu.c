#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>

paddr_t isa_mmu_translate(vaddr_t vaddr, int type, int len) {
  paddr_t pg_table = paddr_read(cpu.cr3 + (vaddr >> 22) * sizeof(paddr_t), sizeof(paddr_t));
  if ((pg_table & 0x1) == 0) {
    Log("cr3: 0x%08x, ptable: 0x%08x, vaddr: 0x%08x, type: %d, len: %d, pc: 0x%08x\n", cpu.cr3, pg_table, vaddr, type, len, cpu.eip);
    assert(false);
  }

  paddr_t pg_base = paddr_read((pg_table & ADDRMASK) + ((vaddr >> 12) & 0x3ff) * sizeof(paddr_t), sizeof(paddr_t));
  if ((pg_base & 0x1) == 0) {
    Log("cr3: 0x%08x, ptable: 0x%08x, pbase: 0x%08x, vaddr: 0x%08x, type: %d, len: %d, pc: 0x%08x\n", cpu.cr3, pg_table, pg_base, vaddr, type, len, cpu.eip);
    assert(false);
  }
  
  pg_base = pg_base & ADDRMASK;
  if ((vaddr & FLAGMASK) + len - 1 > FLAGMASK)
    return pg_base | MEM_RET_CROSS_PAGE;
  return pg_base | MEM_RET_OK;
}
