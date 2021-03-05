#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"
#include "i8042.h"
#include "timer.h"

extern unsigned timer_counter;
extern int timer_hook_id;
extern uint8_t kbd_output_buffer;
extern int kbd_hook_id;

uint32_t sys_inb_count = 0;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
    int ipc_status, r, code_size;
    uint8_t kbd_bitmask = BIT(kbd_hook_id);
    message msg;

    bool doubleIteration = false, make = false;
    uint8_t scancode[2];

    if(kbd_subscribe_int()!=0){return 1;}

    while(kbd_output_buffer != 0x81){
        // Get a request message
        if((r=driver_receive(ANY, &msg, &ipc_status)) !=0 ){
            printf("driver_receive failed with %d\n", r);
            continue;
        }
        if(is_ipc_notify(ipc_status)) //received notification
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: //hardware interrupt notification
                    if (msg.m_notify.interrupts & kbd_bitmask) {
                        sys_inb_count += 2;  //because we're calling kbc_ih (which calls sys_inb twice in total)
                        kbc_ih(); //call interrupt handler

                        if(!doubleIteration){ //if it's our first iteration
                          scancode[0] = kbd_output_buffer;  //save the read scancode
                          if(kbd_output_buffer == 0xE0){            //if it's a 2 byte code
                            doubleIteration = true;             //we'll iterate again
                            code_size = 2;                           //and the size is 2
                            continue; //repeat the loop
                          }
                          else{                                 //it's a 1 byte code
                            code_size = 1;                           //set size to 1
                            if((kbd_output_buffer & BIT(7)) != 0){  //if BIT7 is set
                              make = false;                     //it's not a make code
                            }
                            else{             //if BIT7 is not set
                              make = true;    //it's a make code
                            }
                          }
                        }
                        else{                 //if it's our second iteration
                          if((kbd_output_buffer & BIT(7)) != 0){  //if BIT7 is set
                            make = false;                     //it's not a make code
                          }
                          else{               //if BIT7 is not set
                            make = true;      //it's a make code
                          }
                          scancode[1] = kbd_output_buffer; //save scancode in the second position of the array
                          doubleIteration = false;     //set the doubleIteration to false
                        }
                        //we can now print the scancode
                        if( kbd_print_scancode(make, code_size, scancode) != 0 ) { return 1; }
                    }
                    break;
                default:
                    break; //no other notifications expected: do nothing
            }

    }
    //print the number of sys_inb calls
    if ( kbd_print_no_sysinb(sys_inb_count) != 0 ) { return 1; }

    //unsubscribe keyboard interrupts
    if(kbd_unsubscribe_int() != 0){return 1;}

    return 0;
}

int(kbd_test_poll)(){
  uint32_t sys_inb_count = 0;
  int code_size;
  bool doubleIteration = false, make = false;
  uint8_t command, scancode[2];


  while(true){

    //tries to read a scancode
    sys_inb_count++;  //because we're calling can_read_scancode (which calls sys_inb once)
    if( can_read_scancode() == 0 ) {
      //if we can read one, we do so
      sys_inb_count++;  //because we're calling util_sys_inb
      if( util_sys_inb(OUT_BUF, &kbd_output_buffer) != 0){
        printf("Could not read the scancode.\n");
        continue;
      }
    }
    else{
      //printf("Reading a scancode is not possible.\n");
      continue;
    }

    //if we're reading the 1st byte of the scancode
    if( !doubleIteration ){
      scancode[0] = kbd_output_buffer;    //save the 1st byte in the array

      //if it's a 2 byte code
      if( kbd_output_buffer == 0xE0 ){
        code_size = 2;                //set the size to 2
        doubleIteration = true;       //we'll need to iterate again to get the 2nd byte
        continue;                     //skip the printing to get the 2nd byte
      }
      //if it's a 1 byte code
      else{
        code_size = 1;                               // set the size to 1
        if((kbd_output_buffer & BIT(7)) != 0){
          make = false;
          }
        else{
          make = true;
        }
      }
    }
    //if we're doing the 2nd iteration of a 2 byte code
    else{
      scancode[1] = kbd_output_buffer;              //we save the 2nd byte in the array
      doubleIteration = false;                  //we will be doing the 1st iteration next time
      if((kbd_output_buffer & BIT(7)) != 0){
        make = false;
        }
      else{
        make = true;
      }
    }

    //if we can't print the scancode
    if(kbd_print_scancode(make, code_size, scancode) != 0) {
      printf("Could not print the scancode.\n");
      continue;
    }

    //if we read the break code of the esc key
    if(kbd_output_buffer == 0x81) { break; }

   /* //we wait 20 ms before trying to get a scancode again
    tickdelay(micros_to_ticks(DELAY_US)); */
  }

  if( kbd_print_no_sysinb(sys_inb_count) != 0 ){
    printf("Could not print the number of sys_inb calls.\n");
    return 1;
  }
  

  //
  if(kbd_issue_command(CTRL_REG, READ_CMD)) return 1;
  
  sys_inb_count += 2;  //because we're calling read_return_value (which calls sys_inb twice)
  if(read_return_value(&command)) return 1;

  uint8_t arg = command | BIT(0);
  
  if(kbd_issue_command(CTRL_REG, WRITE_CMD)) return 1;
  
  if(kbd_issue_command(IN_BUF, arg)) return 1;

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
    int ipc_status, r, code_size;
    uint8_t kbd_bitmask = BIT(kbd_hook_id), timer_bitmask = BIT(timer_hook_id);
    message msg;

    uint32_t sys_inb_count = 0;

    bool doubleIteration = false, make = false;
    uint8_t scancode[2];

    if(timer_subscribe_int(&timer_bitmask)!=0){return 1;}
    if(kbd_subscribe_int()!=0){return 1;}

    while((kbd_output_buffer != 0x81) && ((timer_counter / 60) < n)){
        // Get a request message
        if((r=driver_receive(ANY, &msg, &ipc_status)) !=0 ){
            printf("driver_receive failed with %d\n", r);
            continue;
        }
        if(is_ipc_notify(ipc_status)) //received notification
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: //hardware interrupt notification
                    if (msg.m_notify.interrupts & timer_bitmask){
                        timer_int_handler();
                    }
                    if (msg.m_notify.interrupts & kbd_bitmask) {
                        //reset timer_counter to 0, because we got an interrupt
                        timer_counter = 0;
                        sys_inb_count += 2;  //because we're calling kbc_ih (which calls sys_inb twice in total)
                        kbc_ih();

                        if(!doubleIteration){
                          scancode[0] = kbd_output_buffer;
                          if(kbd_output_buffer == 0xE0){
                            doubleIteration = true;
                            code_size = 2;
                            continue;
                          }
                          else{
                            code_size = 1;
                            if((kbd_output_buffer & BIT(7)) != 0){
                              make = false;
                            }
                            else{
                              make = true;
                            }
                          }
                        }
                        else{
                          if((kbd_output_buffer & BIT(7)) != 0){
                            make = false;
                          }
                          else{
                            make = true;
                          }
                          scancode[1] = kbd_output_buffer;
                          doubleIteration = false;
                        }

                        if( kbd_print_scancode(make, code_size, scancode) != 0 ) { return 1; }
                    }
                    break;
                default:
                    break; //no other notifications expected: do nothing
            }

    }
    if ( kbd_print_no_sysinb(sys_inb_count) != 0 ) { return 1; }

    if(timer_unsubscribe_int() != 0){ return 1; }
    if(kbd_unsubscribe_int() != 0){ return 1; }

    return 0;

}
