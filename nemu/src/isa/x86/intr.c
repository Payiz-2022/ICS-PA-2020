#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  assert(0);
  vaddr_t intr_gate = cpu.idtr.base + NO * 64;
  uint32_t gate_l = vaddr_read(intr_gate, 2);
  uint32_t gate_h = vaddr_read(intr_gate + 4, 4);
  if ((gate_h & 0x8000) == 0) return;

  vaddr_t intr_addr = ((gate_h >> 16) << 16) | gate_l;

  rtl_li(s, s0, cpu.eflags.val);
  rtl_push(s, s0);
  rtl_li(s, s0, cpu.cs);
  rtl_push(s, s0);
  rtl_li(s, s0, cpu.pc);
  rtl_push(s, s0);

  rtl_j(s, intr_addr);
}

void query_intr(DecodeExecState *s) {
  TODO();
}
