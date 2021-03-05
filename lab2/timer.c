#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"
#include "timer.h"

int timer_counter = 0;
int timer_hook_id = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  uint8_t preserve = 0;
  if( timer_get_conf(timer, &preserve) != 0 ) { return 1; }
  preserve &= (BIT(3) | BIT(2) | BIT(1) | BIT(0));

  uint16_t newCount = TIMER_FREQ / freq;
  uint8_t lsb = 0, msb = 0;
  if( util_get_LSB(newCount, &lsb) != 0 ) { return 1; }
  if( util_get_MSB(newCount, &msb) != 0 ) { return 1; }

  uint8_t controlWord = TIMER_LSB_MSB | preserve;
  switch(timer){
    case 0:
      controlWord |= TIMER_SEL0;
      break;
    case 1:
      controlWord |= TIMER_SEL1;
      break;
    case 2:
      controlWord |= TIMER_SEL2;
      break;
  }

  if( sys_outb(TIMER_CTRL, controlWord) != 0 ) { return 1; }
  if( sys_outb(TIMER_0 + timer, lsb) != 0 ) { return 1; }
  if( sys_outb(TIMER_0 + timer, msb) != 0 ) { return 1; }

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = BIT(timer_hook_id);
  return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id);
}

int (timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&timer_hook_id);
}

void (timer_int_handler)() {
  timer_counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  //create readback command
  uint8_t readbackCommand = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  //write readback command to the control register
  if ( sys_outb(TIMER_CTRL, readbackCommand) != 0 ) { return 1; }

  //retrieve information from the timer's port
  if( util_sys_inb(TIMER_0 + timer, st) != 0 ) { return 1; }

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  union timer_status_field_val conf;
  switch(field){
    case tsf_all:
      conf.byte = st;
      break;
    case tsf_initial:
      conf.in_mode = st & (BIT(5) | BIT(4));
      conf.in_mode = conf.in_mode >> 4;
      break;
    case tsf_mode:
      conf.count_mode = st & (BIT(3) | BIT(2) | BIT(1));
      conf.count_mode = conf.count_mode >> 1;
      if( conf.count_mode == EXT_TIMER_RATE_GEN ) { //if the mode is 110 -> 010
        conf.count_mode = TIMER_RATE_GEN;
      }
      else{ //if the mode is 111 -> 011, else we keep it as it is
        conf.count_mode = (conf.count_mode == EXT_TIMER_SQR_WAVE) ? TIMER_SQR_WAVE : conf.count_mode;
      }
      break;
    case tsf_base:
      conf.bcd = st & BIT(0)  ;
      break;
  }
  
  if( timer_print_config(timer, field, conf) != 0 ) { return 1; }

  return 0;
}
