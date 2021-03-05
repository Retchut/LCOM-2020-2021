#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

extern int timer_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

//tests reading the config
int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  uint8_t st = 0; //initializes status
  if ( timer_get_conf(timer, &st) != 0 ) { return 1; }            //retrieves the config
  if ( timer_display_conf(timer, st, field) != 0 ) { return 1; }  //displays the config

  return 0;
}

//tests setting the timer frequency
int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  return timer_set_frequency(timer, freq);
}

//tests timer interrupts
int(timer_test_int)(uint8_t time) {
  int ipc_status, r, seconds_elapsed = 0;
  message msg;
  uint8_t timer_bitmask;

  if( timer_subscribe_int(&timer_bitmask) != 0) { return 1; }

  while(seconds_elapsed < time) { /* You may want to use a different condition */
  
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:	/* hardware interrupt notification */
          if (msg.m_notify.interrupts & timer_bitmask) {

            //increments timer_counter
            timer_int_handler();        

            //updates seconds elapsed
            seconds_elapsed = timer_counter / 60;

            //if 1 whole second has passed (timer frequency is 60Hz)
            if(timer_counter % 60 == 0){
              timer_print_elapsed_time();
            }
          }
          break;
        default:
          break;
      }
    }
    else {
    }
  }

  if( timer_unsubscribe_int() != 0 ) { return 1; }

  return 0;
}
