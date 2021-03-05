#include "mouse.h"
#include "utils.h"

uint8_t ms_output_buffer = 0;
int ms_hook_id = 2;
bool valid_ms_data = true;

//Function used to subscribe keyboard interrupts
int ms_subscribe_int(){
    if(sys_irqsetpolicy(MS_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &ms_hook_id) != 0){
        printf("sys_irqsetpolicy failed\n");
        return 1;
    }
    return 0;
}

//Function used to unsubscribe keyboard interrupts
int ms_unsubscribe_int() {
    if(sys_irqrmpolicy(&ms_hook_id) != 0){
        printf("sys_irqrmpolicy failed\n");
        return 1;
    }
    return 0;
}

//Function that checks whether or not we can write to the ctrl reg, to input commands
int can_write_to_ctrl_reg(){
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

//Function which issues a command to the kbc, as argument to the 0xD4 command
int ms_issue_arg_command(uint8_t argument){
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
                //printf("Acknowledged.\n");
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

//Function which enables mouse data reporting, and subscribes mouse interrupts
int ms_init(){
    //enables mouse data_reporting
    if( ms_issue_arg_command(ENABLE_DATA_REPORTING) != 0) {
        printf("Could not enable data reporting.\n");
        return 1;
    }
    //subscribes mouse interrupts
    if ( ms_subscribe_int() != 0 ) {
        return 1;
    }
    return 0;
}

//Read data from the output buffer, as long as it's full, and don't do anything with it
int mouse_read_discard(){
    int counter = 0;

    while(counter < TIMEOUT){

        //increment counter
        counter++;

        //reads status
        uint8_t status = 0;
        util_sys_inb(STATUS_REG, &status);

        //if the ob_status bit on the readstatus is not 0, the buffer is full, and we can read data
        if( (status & OB_STATUS) != 0 ){

            //reads output buffer
            util_sys_inb(OUT_BUF, &ms_output_buffer);
            printf("ms_output_buffer: %d\n", ms_output_buffer);
        }

    }

    return 0;   //if we timed out
}

//Function which disables mouse data reporting, and unsubscribes mouse interrupts
int ms_reset(){

    //unsubscribe mouse interrupts
    ms_unsubscribe_int();

    //write command to disable data reporting
    if( ms_issue_arg_command(DISABLE_DATA_REPORTING) != 0) {
        printf("Could not disable data reporting.\n");
        return 1;
    }

    if( mouse_read_discard() != 0){
        return 1;
    }

    return 0;
}

//mouse input handler
void (mouse_ih)(){
    while (true){

        //reads status
        uint8_t status = 0;
        util_sys_inb(STATUS_REG, &status);

        //if the ob_status bit on the readstatus is not 0, the buffer is full, and we can read data
        if( (status & OB_STATUS) != 0 ){

            //if bit 5 is not 0, the buffer will contain mouse data, so we can read
            if( (status & AUX_OUT_BUF_FULL) != 0  ){

                //reads output buffer
                util_sys_inb(OUT_BUF, &ms_output_buffer);

                //unless the parity and timeout error bits are both 0, the data is invalid, so we don't use it
                if( (status & (TIMEOUT_ERROR | PARITY_ERROR)) != 0 ){
                    valid_ms_data = false;
                    printf("Invalid packet data received.\n");
                }
            }
        }

        return;
    }
}

//parse the received packet
void parse_packet(struct packet* toParse){
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
