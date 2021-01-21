#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr) {
  vaddr_t intr_gate = cpu.idtr.base + NO * 8;

  uint32_t gate_l = vaddr_read(intr_gate, 2);
  uint32_t gate_h = vaddr_read(intr_gate + 4, 4);
  if ((gate_h & 0x8000) == 0) return;

  vaddr_t intr_addr = ((gate_h >> 16) << 16) | gate_l;

  rtl_push(s, &cpu.eflags.val);

  rtl_li(s, s0, cpu.cs);
  rtl_push(s, s0);

  rtl_li(s, s0, ret_addr);
  printf("[raise_intr] eip: 0x%08x\n", ret_addr);
  rtl_push(s, s0);

  rtl_j(s, intr_addr);
}

void query_intr(DecodeExecState *s) {
  rtl_pop(s, s0);
  printf("[iret] eip: 0x%08x\n", *s0);
  rtl_jr(s, s0);

  rtl_pop(s, s0);
  cpu.cs = *s0;

  rtl_pop(s, &cpu.eflags.val);
}
