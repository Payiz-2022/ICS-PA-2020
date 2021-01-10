#include <common.h>
#include "syscall.h"

#define call_return(val) c->GPRx = val;

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_exit: halt(0); break;
    case SYS_yield: yield(); call_return(0); break;
    case SYS_write:
      if (c->GPR2 == 1 || c->GPR2 == 2) {
        // Log("sys_write: fd: %d  buf_addr: 0x%x  count: %d  reg: 0x%x", a[1], a[2], a[3], c->esi);
        char *buf = (void*)a[2];
        for (int i = 0; i < a[3]; i++) {
          putch(buf[i]);
        }
        call_return(0);
      } else {
        call_return(-1);
      }
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
