#include "lcom/lcf.h"

#include "menu.h"
#include "logic.h"
#include "logic_defines.h"
#include "scancode_defines.h"
#include "sprite.h"

//extern program state variable
extern state program_state;

//Extern menu images
extern struct Image menu_img[7];

//Menu object
MenuItems* menu_obj = NULL;

//Function which prepares the menu
int prepare_menu(){

  //Draw menu on the background buffer
  if( draw_menu_background() != 0 ){
    printf("Could not draw the game background on the background buffer.\n");
    return 1;
  }

  //Allocate memory for the Menu object
  menu_obj = (MenuItems *) malloc(sizeof(MenuItems));

  //Check if the allocation was unsuccessful
  if( menu_obj == NULL ){
    printf("Failed alocating the menu memory.\n");
    return 1;
  }
  
  //Initialize menu member variables
  menu_obj -> title_v1 = true;
  menu_obj -> p_button = &menu_img[p_butt];
  menu_obj -> play_button_width = (menu_obj -> p_button) -> image_info.width;
  menu_obj -> play_button_height = (menu_obj -> p_button) -> image_info.height;
  menu_obj -> play_button_x = 6 * (menu_obj -> play_button_height);
  menu_obj -> play_button_y = 7 * (menu_obj -> play_button_height);
  menu_obj -> h_button = &menu_img[h_butt];
  menu_obj -> highscore_width = (menu_obj -> h_button) -> image_info.width;
  menu_obj -> highscore_height = (menu_obj -> h_button) -> image_info.height;
  menu_obj -> highscore_x = menu_obj -> play_button_x;
  menu_obj -> highscore_y = menu_obj -> play_button_y + menu_obj -> play_button_height + 10;

  return 0;
}

//Function which closes the menu
void close_menu(){

  //free the menu object memory
  free(menu_obj);
}

int menu_loop(int tick){
  
  //EVERY 90 FRAMES
  if( tick % 90 == 0 ){
    change_title_animation();
  }

  //draw the menu elements
  if( draw_menu_elements() != 0 ){
    printf("Error drawing the menu elements.\n");
    return 1;
  }

  //update the vram
  update_vram();

  return 0;
}

int menu_kbd_handler(uint8_t scancode){
  switch (scancode)
  {

    case ESC_BREAKCODE:

      //change the program state to the appropriate one
      program_state = close_program;

      //try to close the menu
      close_menu();

      break;

    default:
      break;

  }

  return 0;
}

int menu_mouse_handler(struct packet *read_packet){

  //if we clicked the left mouse button
  if(read_packet->lb){

    //change the program state to the appropriate one
    program_state = menu_option(menu_obj);

    //if we're not staying inside the menu
    if( program_state != menu ){

      //try to close the menu
      close_menu();
      
    }

  }

  if(read_packet->rb){
    
    //change the program state to the appropriate one
    program_state = close_program;

    //try to close the menu
    close_menu();

  }

  return 0;
}
