#include <isa.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
#include "difftest.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  printf("DIFFTEST CHECKREGS ref_r: 0x%p  pc: 0x%x\n", ref_r, pc);
  if (memcmp((void*)ref_r, (void*)&cpu, 8 * sizeof(rtlreg_t)) == 0)
    return true;
  return false;
}

void isa_difftest_attach() {
}
