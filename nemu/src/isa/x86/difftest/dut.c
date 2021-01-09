#include <isa.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
#include "difftest.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if (ref_r->ebx == cpu.ebx && ref_r->ecx == cpu.ecx && ref_r->edx == cpu.edx) return true;
  // if (memcmp((void*)(ref_r->gpr), (void*)cpu.gpr, 8 * sizeof(rtlreg_t)) == 0 
  //     && ref_r->eip == cpu.eip)
  //   return true;
  return false;
}

void isa_difftest_attach() {
}
