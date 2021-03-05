// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "video_gr.h"
#include "keyboard.h"

extern uint8_t kbd_output_buffer;
extern int kbd_hook_id;
extern int timer_counter, timer_hook_id;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {

  //initialize graphics mode
  if( vg_init(mode) == NULL ){
    printf("Failed setting the video mode.\n");
    return 1;
  }

  sleep(delay);

  //return to minix's text mode
  if( vg_exit() != 0 ) {
    printf("vg_exit() failed.\n");
    return 1;
  }

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

  //initiate graphics mode
  if( vg_init(mode) == NULL ){
    printf("Failed setting the video mode.\n");
    return 1;
  }

  //draw rectangle
  if( vg_draw_rectangle(x, y, width, height, color) != 0 ) {
    printf("Error drawing rectangle.\n");
    return 1;
  }

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
                  }
                  break;
              default:
                  break; //no other notifications expected: do nothing
          }

  }

  //unsubscribe keyboard interrupts
  if(kbd_unsubscribe_int() != 0){return 1;}

  if( vg_exit() != 0 ) {
    printf("vg_exit() failed.\n");
    return 1;
  }

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {

  //initialize graphics mode
  if( vg_init(mode) == NULL ){
    printf("Failed setting the video mode.\n");
    return 1;
  }

  //draw grid
  if( draw_filled_grid(no_rectangles, first, step) != 0 ){
    printf("Error drawing pattern of rectangles.\n");
    return 1;
  }

  //lab3 interrupt loop
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
                  }
                  break;
              default:
                  break; //no other notifications expected: do nothing
          }

  }

  //unsubscribe keyboard interrupts
  if(kbd_unsubscribe_int() != 0){return 1;}

  //return to minix's text mode
  if( vg_exit() != 0 ) {
    printf("vg_exit() failed.\n");
    return 1;
  }
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  
  //initialize graphics mode
  if( vg_init(0x105) == NULL ) {
    printf("Error initiating graphics mode.\n");
    return 1;
  }

  //enum that holds data related to the xpm img
  xpm_image_t img_info;

  //reads the xpm, updates img_info with the info and returns
  //the address of the array of the image colors
  uint8_t *img_color_map = xpm_load(xpm, XPM_INDEXED, &img_info);

  //draw the xpm image
  if( draw_xpm(img_info, img_color_map, x, y) != 0 ) {
    return 1;
  }
  
  //lab3 interrupt loop
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
                  }
                  break;
              default:
                  break; //no other notifications expected: do nothing
          }
  }

  //unsubscribe keyboard interrupts
  if(kbd_unsubscribe_int() != 0){return 1;}

  //return to minix's text mode
  if( vg_exit() != 0 ) {
    printf("vg_exit() failed.\n");
    return 1;
  }
  
  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {
  
  //initialize graphics mode
  if( vg_init(0x105) == NULL ) {
    printf("Error initiating graphics mode.\n");
    return 1;
  }

  //enum that holds data related to the xpm img
  xpm_image_t img_info;

  //reads the xpm, updates img_info with the info and returns
  //the address of the array of the image colors
  uint8_t *img_color_map = xpm_load(xpm, XPM_INDEXED, &img_info);

  //draw the xpm image
  uint16_t current_x = xi;
  uint16_t current_y = yi;

  bool horizontal_movement;
  //check if we have vertical or horizontal movement
  if(abs(yf - yi) == 0){
    horizontal_movement = true;
  }
  else{
    horizontal_movement = false;
  }

  //check our mode. if we add speed, we add it at every new frame. else, we move 1 pixel every abs(speed) frames
  bool add_speed;
  int frame_no;   //number of frames after which we should increment 1 byte, if we are not adding the speed
  if(speed > 0){
    add_speed = true;
  }
  else{
    add_speed = false;
    frame_no = abs(speed);
  }

  //draw image at the starting position
  if( draw_xpm(img_info, img_color_map, current_x, current_y) != 0 ) {
    return 1;
  }

  //for interrupts
  int ipc_status, r, code_size;
  uint8_t timer_bitmask= BIT(timer_hook_id);
  uint8_t kbd_bitmask = BIT(kbd_hook_id);
  message msg;

  //setup framerate
  uint8_t number_of_ticks = 60 / fr_rate; //60 ticks / frames_per_second

  bool doubleIteration = false, make = false;
  uint8_t scancode[2];

  if( timer_subscribe_int(&timer_bitmask) != 0) { return 1; }

  if( kbd_subscribe_int() != 0 ){return 1;}

  while(kbd_output_buffer != 0x81) { /* You may want to use a different condition */

    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:	/* hardware interrupt notification */
          if (msg.m_notify.interrupts & timer_bitmask) {

            //increments timer_counter  (we'll never be at the 0 iteration, so 
            //we draw the first position outside)
            timer_int_handler();

            //if we have gone through enough ticksto make one frame
            if(timer_counter % number_of_ticks == 0){

              //depending on the mode, we calculate movement in a different way
              //if we add speed's value every frame
              if(add_speed){

                if( erase_xpm(img_info, current_x, current_y) != 0 ) {
                  return 1;
                }
                //if we're doing horizontal movement
                if(horizontal_movement){
                  //check if the image would go past the final coordinates
                  if(current_x + speed > xf) current_x = xf;
                  else current_x += speed;
                }
                else{ //if we're doing vertical movement
                  //check if the image would go past the final coordinates
                  if(current_y + speed > yf) current_y = yf;
                  else current_y += speed;
                }
                //draw image at the current position
                if( draw_xpm(img_info, img_color_map, current_x, current_y) != 0 ) {
                  return 1;
                }
              }
              else{

                //if we have waited frame_no frames
                if(timer_counter % frame_no == 0){

                  if( erase_xpm(img_info, current_x, current_y) != 0 ) {
                    return 1;
                  }
                  //if we're doing horizontal movement
                  if(horizontal_movement){
                    //check if the image would go past the final coordinates
                    if(current_x + 1 > xf) current_x = xf;
                    else current_x ++;
                  }
                  else{ //if we're doing vertical movement
                    //check if the image would go past the final coordinates
                    if(current_y + 1 > yf) current_y = yf;
                    else current_y ++;
                  }
                }
                if( draw_xpm(img_info, img_color_map, current_x, current_y) != 0 ) {
                  return 1;
                }
              }
            }
          }
          if (msg.m_notify.interrupts & kbd_bitmask) {
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
          }
          break;
        default:
          break;
      }
    }
    else {
    }
  }

  //unsubscribe keyboard interrupts
  if( kbd_unsubscribe_int() != 0 ){return 1;}

  if( timer_unsubscribe_int() != 0 ) { return 1; }

  //return to minix's text mode
  if( vg_exit() != 0 ) {
    printf("vg_exit() failed.\n");
    return 1;
  }

  return 0;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
