#ifndef ARCH_H__
#define ARCH_H__

struct Context {
  uintptr_t e;
  uintptr_t eflags, cs, eip;
  void *cr3;
  int irq;
  uintptr_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
};

// struct Context {
//   uintptr_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
//   int irq;
//   void *cr3;
//   uintptr_t eip, cs, eflags;
// };

// struct Context {
//   uintptr_t esi, ebx, eax, eip, edx, eflags, ecx, cs, esp, edi, ebp;
//   void *cr3;
//   int irq;
// };

#define GPR1 eax
#define GPR2 eip
#define GPR3 eip
#define GPR4 eip
#define GPRx eip

#endif
