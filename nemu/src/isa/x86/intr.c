#include <cpu/exec.h>
#include "local-include/rtl.h"

#define IRQ_TIMER 0x20

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr) {
  vaddr_t intr_gate = cpu.idtr.base + NO * 8;

  uint32_t gate_l = vaddr_read(intr_gate, 2);
  uint32_t gate_h = vaddr_read(intr_gate + 4, 4);
  if ((gate_h & 0x8000) == 0) return;

  vaddr_t intr_addr = ((gate_h >> 16) << 16) | gate_l;

  rtl_push(s, &cpu.eflags.val);
  cpu.eflags.IF = 0;

  rtl_li(s, s0, cpu.cs);
  rtl_push(s, s0);

  rtl_li(s, s0, ret_addr);
  rtl_push(s, s0);

  rtl_j(s, intr_addr);
}

void restore_intr(DecodeExecState *s) {
  rtl_pop(s, s0);
  rtl_jr(s, s0);

  rtl_pop(s, s0);
  cpu.cs = *s0;

  rtl_pop(s, &cpu.eflags.val);
}

void query_intr(DecodeExecState *s) {
  if (cpu.eflags.IF) printf("IF: %d", cpu.eflags.IF);
  if (cpu.INTR & cpu.eflags.IF) {
    cpu.INTR = false;
    raise_intr(s, IRQ_TIMER, cpu.pc);
    update_pc(s);
  }
}
