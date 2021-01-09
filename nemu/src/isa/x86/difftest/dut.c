#include <isa.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
#include "difftest.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if (memcmp((void*)ref_r + 1, (void*)&cpu + 1, 8 * sizeof(rtlreg_t) - 1) == 0 
      && ref_r->eip == cpu.eip)
    return true;
  return false;
}

void isa_difftest_attach() {
}
