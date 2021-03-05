// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

#include <stdbool.h>
#include <stdint.h>

// Any header files included below this line should have been created by you

#include "sprite.h"     //also includes video.h
#include "logic.h"  //REMOVE LATER
#include "proj_timer.h"
#include "keyboard.h"
#include "mouse.h"
#include "rtc.h"
#include "game.h"
#include "menu.h"
#include "highscore.h"
#include "logic_defines.h"

//timer interrupts globals
extern int timer_counter, timer_hook_id;

//keyboard globals
extern uint8_t kbd_output_buffer;
extern int kbd_hook_id;
extern bool valid_kbd_data;

//mouse globals
extern uint8_t ms_output_buffer;
extern int ms_hook_id;
extern bool valid_ms_data;

//controls the game state
state program_state = menu_open;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(proj_main_loop)(int argc, char *argv[]) {

  //for all interrupts
  int ipc_status, r;
  message msg;

  //for timer interrupts
  uint8_t timer_bitmask = BIT(timer_hook_id);
  
  //for keyboard interrupts
  uint8_t kbd_bitmask = BIT(kbd_hook_id);

  //for mouse interrupts
  uint8_t ms_bitmask = BIT(ms_hook_id);
  unsigned array_pos = 0;
  struct packet read_packet;

  //initialize variables for interacting with the highscores file
  char *names[5];                               //names before load
  for(int i = 0; i < 5; i++){                   //for each name string
    names[i] = (char *) malloc(6*sizeof(char)); //allocate 6 bytes (5 chars + null char)
    strcpy(names[i], "NONE");                   //fill it with the placeholder string NONE
  }
  int times[MAX_HIGHSCORES] = {0};              //times (filled with 0s at the start)
  int scores[MAX_HIGHSCORES] = {0};             //highscores (filled with 0s at the start)
  int new_score = 0;                            //new score the player will obtain
  char new_name[HS_NAME_MAX_SIZE];              //name the player will input later
  int new_name_it = 0;                          //iterator for the new name
  memset(new_name, '\0', HS_NAME_MAX_SIZE);     //clean the new name

  //load the highscores from our highscores file
  if( load_highscores(names, times, scores) != 0 ){
    printf("Unable to load the highscores.\n");
    return 1;
  }

  //generate the cursor object
  if( generate_cursor() != 0 ){
    printf("Unable to generate the cursor object.\n");
    return 1;
  }
  
  /*
        INITIALIZE GRAPHICS CARD AND SUBSCRIBE DEVICE INTERRUPTS
  */
  //initiate graphics
  if( graphics_init() != 0 ) { return 1; }
  //subscribe timer interrupts
  if( proj_timer_subscribe_int() != 0) { return 1; }
  //subscribe keyboard interrupts
  if( kbd_subscribe_int() != 0 ){ return 1; }
  //initiate mouse
  if( ms_init() != 0 ){ return 1; }
  
  //load images to be used in the game
  load_images();

  while( program_state != close_program ) {

    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:	/* hardware interrupt notification */

          //Received timer interrupt
          if (msg.m_notify.interrupts & timer_bitmask) {

            //increments timer_counter
            timer_int_handler();

            switch(program_state){
              
              //if we want to open the menu
              case menu_open:

                //destroy game objects
                end_game();

                //reset the iterator and the new name string to their initial values
                new_name_it = 0;
                memset(new_name, '\0', HS_NAME_MAX_SIZE);
                
                //prepare to display the menu
                if( prepare_menu() != 0 ){
                  printf("Error preparing the menu.\n");
                  return 1;
                }

                //change the program state
                program_state = menu;
                break;


              //if we're on the menu
              case menu:

                //menu loop
                if( menu_loop(timer_counter) != 0 ){
                  return 1;
                }
                break;


              //if we want to open the highscore screen
              case highscore_open:

                //prepare to display the highscore menu
                if( prepare_highscore_menu(names, times, scores) != 0 ){
                  return 1;
                }

                //change the program state
                program_state = highscore;
                break;


              //if we're on the highscore screen
              case highscore:

                //highscore menu loop
                if( highscore_menu_loop() != 0 ){
                  return 1;
                }
                break;

              
              //if we want to start the game
              case game_start:

                //prepare before the game starts
                if( prepare_game() != 0 ){
                  printf("Error preparing the game.\n");
                  return 1;
                }

                //change the program state
                program_state = game_ok;
                break;


              //if we're in the middle of the game
              case game_ok:

                //core gameplay loop
                if( game_loop(timer_counter) != 0 ){
                  return 1;
                }
                break;


              //if the game ended
              case game_over:

                //save score, if higher than the smallest saved score
                new_score = check_new_highscore(scores);

                //if there's no new highscore, we just end the game as usual
                if(new_score == 0){
                  
                  //change program state to menu_open
                  program_state = menu_open;
                }
                else{ //if there is a new highscore, we store it in our arrays

                  //Prepare the screen where we'll receive a name from the player
                  //To save the new highscore with
                  if( prepare_new_highscore() != 0 ){
                    return 1;
                  }
                  
                  //change program state to menu_open
                  program_state = new_hs;

                }                
                break;


              //if we want to save a new highscore
              case new_hs:

                //core loop for editing the highscores
                if( new_highscore_loop(new_name) != 0 ){
                  return 1;
                }
                break;

              //if we received a valid name, and are saving the highscore
              case save_new_hs:
              
                if( read_time() != 0 ){
                  return 1;
                }

                int time = parse_time();

                if( add_new_highscore(names, times, scores, new_name, time, new_score) != 0 ){
                  return 1;
                }

                program_state = menu_open;
                break;

              //if we want to close the program
              case close_program:
                break;
            }
            
          }
          
          //Received keyboard interrupt
          if (msg.m_notify.interrupts & kbd_bitmask) {

            kbc_ih(); //call interrupt handler

            //if we read valid kbd data
            if(valid_kbd_data == true && kbd_output_buffer != 0xE0){

              switch(program_state){


              //if we want to open the menu
              case menu_open:
                break;


              //if we're on the menu
              case menu:
              
                //call the menu screen's keyboard handler
                if( menu_kbd_handler(kbd_output_buffer) != 0 ) {
                  return 1;
                }
                break;


              //if we want to open the highscore screen
              case highscore_open:
                break;


              //if we're on the highscore screen
              case highscore:
              
                //call the highscore screen's keyboard handler
                highscore_menu_kbd_handler(kbd_output_buffer);
                break;


              //if we want to start the game
              case game_start:
                break;


              //if we're in the middle of the game
              case game_ok:
              
                //call the game's keyboard handler
                game_kbd_handler(kbd_output_buffer);
                break;


              //if the game ended
              case game_over:
                break;


              //if we want to save a new highscore
              case new_hs:
                new_highscore_kbd_handler(kbd_output_buffer, new_name, &new_name_it);
                break;

                
              case save_new_hs:
                break;


              //if we want to close the program
              case close_program:
                break;
              }
              
            }
            else{
              //if we read invalid kbd data, we set the flag to true
              //so we can work with the next read data, if it's valid
              valid_kbd_data = true;
            }
          }

          //Received mouse interrupt
          if(msg.m_notify.interrupts & ms_bitmask){

            mouse_ih(); //retrieves 1/3 of a packet

            //checks if the data we read is valid
            if( valid_ms_data == true ){
              //if we're trying to read the 1st byte
              if( array_pos == 0){

                //checks if we read the 1st byte of the packet
                if( (ms_output_buffer & BIT(3)) == 0) { //the 1st byte's bit 3 is ALWAYS 1
                  continue; //we read again, ignoring what we just read
                }
              }

              read_packet.bytes[array_pos] = ms_output_buffer; //saves to pos 0/1/2 of the array
              array_pos++;                                    //gets the next pos( 1/2/3 )

              //after assembling a full packet
              if(array_pos == 3){
                array_pos = 0;                    //set the position back to 0
                parse_packet(&read_packet);        //parse the assembled packet

                //update the mouse position
                move_cursor(read_packet.delta_x, read_packet.delta_y);

                switch(program_state){


                  //if we want to open the menu
                  case menu_open:
                    break;


                  //if we're on the menu
                  case menu:
                  
                    //call the menu screen's mouse handler
                    if( menu_mouse_handler(&read_packet) != 0 ) {
                      return 1;
                    }
                    break;


                  //if we want to open the highscore screen
                  case highscore_open:
                    break;


                  //if we're on the highscore screen
                  case highscore:

                    //call the highscore screen's mouse handler
                    highscore_menu_mouse_handler(&read_packet);
                    break;


                  //if we want to start the game
                  case game_start:
                    break;


                  //if we're in the middle of the game
                  case game_ok:
                  
                    //call the game's mouse handler
                    if( game_mouse_handler(&read_packet) != 0 ) {
                      return 1;
                    }
                    break;


                  //if the game ended
                  case game_over:
                    break;


                  //if we want to save a new highscore
                  case new_hs:
                    new_highscore_mouse_handler(&read_packet, new_name, &new_name_it);
                    break;
                    
                
                  case save_new_hs:
                    break;


                  //if we want to close the program
                  case close_program:
                    break;
                }

              }
            }
            else{ //if we received invalid data (valid_ms_data = false)
              valid_ms_data = true;             //set bool back to true
              array_pos = 0;                    //set the position back to 0 (discard packet)
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

  printf("Ended interrupt loop.\n");

  //unsubscribe keyboard interrupts
  if( kbd_unsubscribe_int() != 0 ) { return 1; }

  //reset mouse after unsubscribing kbd interrupts to prevent bricking
  if( ms_reset() != 0 ){ return 1; }

  //unsubscribe timer interrupts
  if( timer_unsubscribe_int() != 0 ) { return 1; }

  //reset video card
  if( vg_exit() != 0 ) { return 1; }

  //Save highscores
  if( save_highscores(names, times, scores) != 0 ){
    printf("Unable to load the highscores.\n");
    return 1;
  }

  printf("closed program\n");

  return 0;

}
