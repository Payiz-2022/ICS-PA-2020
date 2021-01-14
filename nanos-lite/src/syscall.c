#include <common.h>
#include <fs.h>
#include "syscall.h"

#define set_return(val) c->GPRx = val;

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  #ifdef DEBUG
    Log("[Syscall] Received syscall (id = %d)", a[0]);
  #endif

  switch (a[0]) {
    case SYS_exit:
      halt(0);
      break;

    case SYS_yield:
      yield();
      set_return(0);
      break;

    case SYS_open:
      set_return(fs_open((const char*)a[1], a[2], a[3]));
      break;

    case SYS_read:
      set_return(fs_read(a[1], (void*)a[2], a[3]));
      break;

    case SYS_write:
      if (c->GPR2 == 1 || c->GPR2 == 2) {
        char *buf = (void*)a[2];
        for (int i = 0; i < a[3]; i++) {
          putch(buf[i]);
        }
        set_return(a[3]);
      } else {
        set_return(fs_write(a[1], (void*)a[2], a[3]));
      }
      break;

    case SYS_close:
      set_return(fs_close(a[1]));
      break;

    case SYS_lseek:
      set_return(fs_lseek(a[1], a[2], a[3]));
      break;

    case SYS_brk:
      set_return(0);
      break;

    case SYS_gettimeofday:
      ;
      struct timeval {
        long int tv_sec;   /* seconds */
        long int tv_usec;  /* and microseconds */
      };
      // set_return(gettimeofday((struct timeval*)a[1], (struct timezone*)a[2]));
      ((struct timeval*)a[1])->tv_usec = io_read(AM_TIMER_UPTIME).us;
      ((struct timeval*)a[1])->tv_sec = ((struct timeval*)a[1])->tv_usec / 1000000;
      set_return(0);
      break;

    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
