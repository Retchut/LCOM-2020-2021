// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "mouse.h"
#include "timer.h"

extern uint8_t ms_output_buffer;
extern int ms_hook_id, timer_hook_id;
extern int timer_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {

  int ipc_status, r;
  uint8_t ms_mask = BIT(ms_hook_id);  //initialize bit mask with the hook_id's bit
  message msg;
  unsigned count = 0, array_pos = 0;
  struct packet readPacket;

  /*    INITIAL CONFIGURATION   */

  //write command to enable data reporting
  if( ms_issue_arg_command(ENABLE_DATA_REPORTING) != 0) {
    printf("Could not enable data reporting.\n");
    return 1;
  }
  //subscribes mouse interrupts
  if ( ms_subscribe_int() != 0 ) { return 1; }


  /*      INTERRUPT LOOP       */

  while(count != cnt){
    //get request message
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {  //received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if(msg.m_notify.interrupts & ms_mask) { //subscribed interrupt          
            //get one third of a packet
            mouse_ih();
            if(array_pos == 0){   //if we're trying to read the 1st byte
              if( (ms_output_buffer & BIT(3)) == 0) { //the 1st byte's bit 3 is ALWAYS 1
                continue; //we read again
              }
            }
            readPacket.bytes[array_pos] = ms_output_buffer; //saves to pos 0/1/2 of the array
            array_pos++;                                    //gets the next pos( 1/2/3 )
            if(array_pos == 3){ //we've got a full packet
              array_pos = 0;    //set the position back to 0
              count++;
              parse_packet(&readPacket);
              mouse_print_packet(&readPacket);
            }
          }
          break;
        default:
          break;  //no other notifications expected: do nothing
        }
      }else{  //received a standard message, not a notification
      //do nothing
    }
  }


  /*    RESETTING MOUSE TO ORIGINAL CONFIGURATION   */

  //unsubscribe mouse interrupts
  ms_unsubscribe_int();

  //write command to disable data reporting
  if( ms_issue_arg_command(DISABLE_DATA_REPORTING) != 0) {
    printf("Could not disable data reporting.\n");
    return 1;
  }

  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  int ipc_status, r;
  uint8_t ms_mask = BIT(ms_hook_id), timer_mask = BIT(timer_hook_id);  //initialize bit mask with the hook_id's bit
  message msg;
  unsigned count = 0, array_pos = 0;
  struct packet readPacket;


  /*    INITIAL CONFIGURATION   */

  //write command to enable data reporting
  if( ms_issue_arg_command(ENABLE_DATA_REPORTING) != 0) {
    printf("Could not enable data reporting.\n");
    return 1;
  }
  //subscribes mouse interrupts
  if ( ms_subscribe_int() != 0 ) { return 1; }
  //subscribes timer interrupts
  if ( timer_subscribe_int(&timer_mask) != 0 ) { return 1; }


  /*      INTERRUPT LOOP       */

  while((timer_counter / 60) < idle_time){
    //get request message
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {  //received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if(msg.m_notify.interrupts & timer_mask) { //subscribed interrupt          
            //increment timer_counter
            timer_int_handler();
          }
          if(msg.m_notify.interrupts & ms_mask) { //subscribed interrupt          
            //reset timer_counter to 0, since we got an interrupt
            timer_counter = 0;
            //get one third of a packet
            mouse_ih();
            if(array_pos == 0){   //if we're trying to read the 1st byte
              if( (ms_output_buffer & BIT(3)) == 0) { //the 1st byte's bit 3 is ALWAYS 1
                continue; //we read again
              }
            }
            readPacket.bytes[array_pos] = ms_output_buffer; //saves to pos 0/1/2 of the array
            array_pos++;                                    //gets the next pos( 1/2/3 )
            if(array_pos == 3){ //we've got a full packet
              array_pos = 0;    //set the position back to 0
              count++;
              parse_packet(&readPacket);
              mouse_print_packet(&readPacket);
            }
          }
          break;
        default:
          break;  //no other notifications expected: do nothing
        }
      }else{  //received a standard message, not a notification
      //do nothing
    }
  }


  /*    RESETTING MOUSE TO ORIGINAL CONFIGURATION   */
  
  //unsubscribe timer interrupts
  timer_unsubscribe_int();
  //unsubscribe mouse interrupts
  ms_unsubscribe_int();
  //write command to disable data reporting
  if( ms_issue_arg_command(DISABLE_DATA_REPORTING) != 0) {
    printf("Could not disable data reporting.\n");
    return 1;
  }

  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* To be completed */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
