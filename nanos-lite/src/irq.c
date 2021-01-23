#include <common.h>
#include <proc.h>

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD: return schedule(c); break;
    case EVENT_SYSCALL: do_syscall(c); break;
    default: panic("Unhandled event ID = %d", e.event);
  }
  printf("Return from syscall irq: 0x%x, cr3: 0x%08x\n", c->irq, c->cr3);
  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
