#include <lcom/lcf.h>

#include "keyboard.h"
#include "i8042.h"

uint8_t kbd_output_buffer;
int kbd_hook_id = 1;
bool valid_kbd_data = true;

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

void (kbc_ih)(){

  while(true){

    //reads status
    uint8_t status = 0;
    util_sys_inb(STATUS_REG, &status);

    //if the ob_status bit on the readstatus is not 0, the buffer is full, and we can read data
    if( (status & OB_STATUS) != 0 ){

      //if bit 5 is 0, the buffer will contain keyboard data, so we can read it
      if( (status & AUX_OUT_BUF_FULL) == 0 ) {

        //reads output buffer
        util_sys_inb(OUT_BUF, &kbd_output_buffer);

        //unless the parity and timeout error bits are both 0, the data is invalid, so we don't use it
        if( (status & (TIMEOUT_ERROR | PARITY_ERROR)) != 0 ) {
          valid_kbd_data = false;
          printf("Invalid scancode data received.\n");
        }
      }
      return;
    }
  }
}
