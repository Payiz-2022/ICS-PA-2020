#ifndef ARCH_H__
#define ARCH_H__

struct Context {
  uintptr_t _esp;
  void *cr3;
  uintptr_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
  int irq;
  uintptr_t eip, cs, eflags;
};

#define GPR1 eax
#define GPR2 eip
#define GPR3 eip
#define GPR4 eip
#define GPRx eip

#endif
