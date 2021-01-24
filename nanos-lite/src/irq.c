#include <common.h>
#include <proc.h>
static int times = 0;
static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_IRQ_TIMER: Log("Received a timer event"); 
      return ++times > 2 ? c : schedule(c);
    case EVENT_YIELD: return schedule(c);
    case EVENT_SYSCALL: do_syscall(c); break;
    default: panic("Unhandled event ID = %d", e.event);
  }
  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
