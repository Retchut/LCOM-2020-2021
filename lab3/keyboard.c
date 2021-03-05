#include <lcom/lcf.h>

#include "keyboard.h"
#include "i8042.h"

uint8_t kbd_output_buffer;
int kbd_hook_id = 1;
extern uint32_t sys_inb_count;

int (kbd_subscribe_int)() {
    if(sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != 0 ){
        printf("sys_irqsetpolicy failed\n");
        return 1;
    }
    return 0;
}

int (kbd_unsubscribe_int)() {
    if(sys_irqrmpolicy(&kbd_hook_id) != 0){
        printf("sys_irqrmpolicy failed\n");
        return 1;
    }
    return 0;
}

int can_read_scancode(){
  uint8_t status = 0;
  //reads status. If it can't, returns 1
  if( util_sys_inb(STATUS_REG,&status) != 0 ) { return 1; }

  //if the ob_status bit on the readstatus is 0, the buffer is empty, and we shouldn't read it
  if( (status & OB_STATUS) == 0 ){ return 1; }

  //if bit 5 is not 0, the buffer will contain mouse data
  if( (status & AUX_OUT_BUF_FULL) != 0 ) { return 1; }

  //unless the parity and timeout error bits are both 0, the data is invalid, and we shouldn't read it
  if( (status & (TIMEOUT_ERROR | PARITY_ERROR)) != 0 ) { return 1; }

  //if it passed all tests, we can read the scancode from the buffer, so we return 0
  return 0;
}

void (kbc_ih)(){

  while(true){
    if( can_read_scancode() == 0 ) {
      if ( util_sys_inb(OUT_BUF, &kbd_output_buffer) == 0) {  
        return;
      }
    }
  }
}

int kbd_issue_command(int port, uint8_t command/*, bool arg*/){

    uint8_t status;
    int tries = 0;

  while(tries < TIMEOUT){

    sys_inb_count++;
    //reads the status register
    if( util_sys_inb(STATUS_REG, &status) != 0 ){
      return 1;
    }
    
    //if the input buffer is empty, and we can write to it
    if( (status & IB_STATUS) == 0) {

      //writes command to the desired port
      if( sys_outb(port, command) != 0 ){
        return 1;
      }

      //waits 20ms to let the kbc handle the command
      tickdelay(micros_to_ticks(DELAY_US));

      return 0;
    }
  
    tries++;
  }

  //if we have timed out
  return TIMED_OUT;
}

int read_return_value(uint8_t *returned_value){

  uint8_t status;
  int tries = 0;

  while( tries < TIMEOUT ) {

    sys_inb_count++;
    //reads the status register
    if( util_sys_inb(STATUS_REG, &status) != 0 ){
      return 1;
    }

    //if the OB_STATUS bit is not set to 1, the OB is empty and we can read from it
    if( (status & OB_STATUS) != 0 ) {

      //if there is no timeout error or parity error, the data is valid
      if( (status & (TIMEOUT_ERROR | PARITY_ERROR)) == 0 ){

        sys_inb_count++;
        //reads what is on the output buffer (our return value from our previously written command)
        if( util_sys_inb(OUT_BUF, returned_value) != 0 ){
          return 1;
        }
      }
    }

    //OB was not empty, or the data was invalid
    tries++;
  }

  //if we have timed out
  return TIMED_OUT;
}

