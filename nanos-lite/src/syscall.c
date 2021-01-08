#include <common.h>
#include "syscall.h"
void do_syscall(Context *c) {
  // uintptr_t a[4];
  // a[0] = c->GPR1;

  switch (c->GPR1) {
    case SYS_exit: halt(0); break;
    case SYS_yield: /*yield();*/ c->GPRx = 0; break;
    default: panic("Unhandled syscall ID = %d", c->GPR1);
  }
}
