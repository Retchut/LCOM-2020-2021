#include "mouse.h"

uint8_t ms_output_buffer = 0;
int ms_hook_id = 2;
//function used to subscribe keyboard interrupts
int (ms_subscribe_int)(){
    if(sys_irqsetpolicy(MS_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &ms_hook_id) != 0){
        printf("sys_irqsetpolicy failed\n");
        return 1;
    }
    return 0;
}

//function used to unsubscribe keyboard interrupts
int (ms_unsubscribe_int)() {
    if(sys_irqrmpolicy(&ms_hook_id) != 0){
        printf("sys_irqrmpolicy failed\n");
        return 1;
    }
    return 0;
}

int (can_read_packet)(){
  uint8_t status = 0;
  //reads status. If it can't, returns 1
  if( util_sys_inb(STATUS_REG,&status) != 0 ) { return 1; }

  //if the ob_status bit on the readstatus is 0, the buffer is empty, and we shouldn't read it
  if( (status & OB_STATUS) == 0 ){ return 1; }

  //if bit 5 is not 1, the buffer will contain keyboard data
  if( (status & AUX_OUT_BUF_FULL) == 0 ) { return 1; }

  //unless the parity and timeout error bits are both 0, the data is invalid, and we shouldn't read it
  if( (status & (TIMEOUT_ERROR | PARITY_ERROR)) != 0 ) { return 1; }

  //if it passed all tests, we can read the scancode from the buffer, so we return 0
  return 0;
}

//mouse input handler
void (mouse_ih)(){
    while (true){
        if( can_read_packet() == 0 ) {
            if( util_sys_inb(OUT_BUF, &ms_output_buffer) == 0 ){
                break;
            }
            printf("Could not read the output.\n");
            continue;
        }
        printf("Packet data is invalid.\n");
    }
    return;
}

//parse the received packet
void (parse_packet)(struct packet* toParse){
    uint8_t byte1 = toParse->bytes[0];
    uint8_t byte2 = toParse->bytes[1];
    uint8_t byte3 = toParse->bytes[2];

    //sets the button click bools (true if different from 0)
    toParse->mb = (byte1 & MMB_CLICK);    //if MMB_CLICK's bit is 1
    toParse->rb = (byte1 & RMB_CLICK);    //if RMB_CLICK's bit is 1
    toParse->lb = (byte1 & LMB_CLICK);    //if LMB_CLICK's bit is 1

    //set X movement
    toParse->delta_x = byte2;    //gets unsigned X movement value
    
    if( (byte1 & X_SIGN) != 0 ){  //if X_SIGN isn't set to 0
        toParse->delta_x -= 256;    //8th bit is 1, and we sign extend
    }

    //set Y movement
    toParse->delta_y = byte3;    //gets unsigned Y movement value

    if( (byte1 & Y_SIGN) != 0 ){  //if Y_SIGN isn't set to 0
        toParse->delta_y -= 256;    //8th bit is 1, and we sign extend
    }
    
    //sets the overflow bools (true if different from 0)
    toParse->y_ov = (byte1 & Y_OVERFLOW); //if Y_OVERFLOW's bit is 1
    toParse->x_ov = (byte1 & X_OVERFLOW); //if X_OVERFLOW's bit is 1
}

int (can_write_to_ctrl_reg)(){
    int counter = 0;
    uint8_t status;

    while(counter < TIMEOUT){
        counter++;

        //reads status
        if( util_sys_inb(STATUS_REG, &status) != 0 ) {
            return 1;   //if sys_inb fails, everything will probably fail too
        }

        //if we can write to the buffer
        if( (status & IB_STATUS) == 0 ) {
            return 0;
        }
    }
    return TIMED_OUT;   //if we timed out
}

int (ms_issue_arg_command)(uint8_t argument){
    uint8_t acknowledgment;

    //check if we can write to the control register
    if( can_write_to_ctrl_reg() != 0 ) {
        printf("Unable to write to the control register.\n");
        return 1;
    }
    
    //try to write the 0xD4 command to the control register
    if( sys_outb(CTRL_REG, WRITE_BYTE_TO_MOUSE) != 0 ) {
        printf("Could not write 0xD4 command to the control register.\n");
        return 1;
    }

    while(true){
        //check if we can write to the control register
        if( can_write_to_ctrl_reg() != 0 ) {
            printf("Unable to write to the control register.\n");
            return 1;
        }

        //write the desired command as an argument to 0xD4
        if( sys_outb(IN_BUF, argument) != 0 ) {
            printf("Could not write the desired command.\n");
            return 1;
        }

        //wait after writing a command
        tickdelay(micros_to_ticks(DELAY_US));
        
        //receive the acknowledgment
        if( util_sys_inb(OUT_BUF, &acknowledgment) != 0 ) {
            printf("Could not receive the acknowledgment.\n");
            return 1;
        }

        //evaluate acknowledgment
        switch (acknowledgment){
            case ACK:
                printf("Acknowledged.\n");
                return 0;

            case ERROR:
                printf("Error sending command.\n");
                return 1;

            case NACK:
                printf("Negative acknowledgment. Retrying.\n");
                break;
        }
    }
}
