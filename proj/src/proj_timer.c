#include "lcom/lcf.h"

#include <stdint.h>

#include "i8254.h"
#include "proj_timer.h"

int timer_counter = 0;
int timer_hook_id = 0;

//Subscribes and enables Timer 0 interrupts
int (proj_timer_subscribe_int)() {
  return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id);
}

//Unsubscribes Timer 0 interrupts
int (timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&timer_hook_id);
}

//Timer 0 interrupt handler
void (timer_int_handler)() {
  timer_counter++;
}
