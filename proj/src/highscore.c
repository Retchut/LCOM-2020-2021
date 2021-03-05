#include "lcom/lcf.h"

#include <ctype.h>
#include <string.h>

#include "highscore.h"
#include "logic.h"
#include "video_defines.h"
#include "logic_defines.h"
#include "scancode_defines.h"
#include "sprite.h"

//extern program state variable
extern state program_state;

const char make_line_1[] = {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'};
const char make_line_2[] = {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l'};
const char make_line_3[] = {'z', 'x', 'c', 'v', 'b', 'n', 'm'};


//Extern images
extern struct Image letters[26];
extern struct Image menu_img[7];

//Function which loads the highscores from the highscores.txt file
int load_highscores(char **n, int *t, int *s){

  //file pointer
  FILE *file;
  //open the file for reading
  file = fopen("home/lcom/labs/proj/highscores.txt", "r");

  //if there was an error opening the file
  if(file == NULL)
  {
    printf("highscores.txt file does not exist. Will be created and filled with the default data.\n");
    return 0;
  }

  //while we can still read
  int iteration = 0;
  while(!feof(file)){

    //load name
    fscanf(file, "%s", n[iteration]);
    //load time
    fscanf(file, "%d", &t[iteration]);
    //load score
    fscanf(file, "%d", &s[iteration]);
    iteration++;
  }

  //close the file
  fclose(file);

  //sort to make sure the rest of the game works correctly
  if( sort_highscores(n, t, s) != 0 ){
    printf("Could not sort the highscores after loading.\n");
    return 1;
  }

  return 0;
}

//Function which sorts the highscores using a simple bubblesort algorythm
int sort_highscores(char **n, int *t, int *s){

  //simple bubblesort
  //iterate through the array MAX_HIGHSCORES times, in reverse order
  for(int i = MAX_HIGHSCORES; i > 1; i--){

    //iterate through the elements from 0 to MAX_HIGHSCORES-1
    for(int j = MAX_HIGHSCORES - 1; j > 0; j--){

      //if our score in the slot j is lower than the score in j+1
      if( s[j] > s[j-1] ){

        //swap the scores
        int temp_score = s[j-1];
        s[j-1] = s[j];
        s[j] = temp_score;

        //swap the names
        char* temp_name = n[j-1];
        n[j-1] = n[j];
        n[j] = temp_name;

        //swap the times
        int temp_time = t[j-1];
        t[j-1] = t[j];
        t[j] = temp_time;
      }
    }
  }
  return 0;

}

//Function which saves the highscores on the highscores.txt file
int save_highscores(char **n, int *t, int *s){

  //file pointer
  FILE *file;
  //open the file for reading
  file = fopen("home/lcom/labs/proj/highscores.txt", "w");

  //if there was an error opening the file
  if(file == NULL)
  {
      printf("Error opening the highscores.txt file.\n");
      return 1;      
  }

  //while we can still write
  for(int i = 0 ; i < MAX_HIGHSCORES; i++){
    //write to file each name, time and score, separated by a space
    fprintf(file, "%s %d %d\n", n[i], t[i], s[i]);
  }

  //close the file
  fclose(file);

  return 0;
}

//Function which prepares the menu
int prepare_highscore_menu(char **n, int *t, int *s){

  //Draw menu on the background buffer
  if( draw_highscore_background(n, t, s) != 0 ){
    printf("Could not draw the game background on the background buffer.\n");
    return 1;
  }

  return 0;
}

//Loop for the highscore menu
int highscore_menu_loop(){

  //Copy the background to the double buffer
  copy_from_background();

  //Draw the cursor object on the double buffer
  if( draw_cursor() != 0 ){
    return 1;
  }

  //update the vram
  update_vram();

  return 0;
}

//Keyboard handler for the highscore menu
void highscore_menu_kbd_handler(uint8_t scancode){

  switch (scancode)
  {
    //if we let go of the esc key
    case ESC_BREAKCODE:
      program_state = menu_open;
    
    //change the state to the appropriate one
      break;
      
    default:
      break;
  }
}

//Mouse handler for the highscore menu
void highscore_menu_mouse_handler(struct packet *read_packet){

  //if we clicked the right mouse button
  if(read_packet->rb){
    
    //change the state to the appropriate one
    program_state = menu_open;

  }

}

//Function which prepares the game to receive a name from the player
int prepare_new_highscore(){
  
  //copy the double buffer into the background buffer
  copy_to_background();

  //write the new_highscore image into the background buffer
  if( draw_new_highscore_screen() != 0 ){
    printf("Could not draw the new highscore screen on the background buffer.\n");
    return 0;
  }

  return 0;
}

//Function which adds a new highscore to the highscore vectors
int add_new_highscore(char **n, int *t, int *s, char *new_name, int new_time, int new_score){

  //copy the new name over to the last position of the names array
  strcpy( n[HS_NAME_MAX_SIZE - 2], new_name);
  
  //save the new time in the last position of the times array
  t[HS_NAME_MAX_SIZE - 2] = new_time;
  
  //save the new score in the last position of the scores array
  s[HS_NAME_MAX_SIZE - 2] = new_score;
  
  //sort the highscores once again
  if( sort_highscores(n, t, s) != 0 ){
    return 1;
  }

  return 0;
}

//Function which draws a name on the double buffer
int draw_new_name(char *to_draw){

  //draw letters from char*
  char current_char;
  int str_pos = 0;
    
  do{

    //get the current char from the string, in lower case
    current_char = (char) tolower( (int) to_draw[str_pos]);
    //get its corresponding image
    Image *current_image = &letters[current_char - 'a'];
    uint16_t current_x = NEW_HS_NAME_X + str_pos * NEW_HS_X_INCREMENT;

    //draw the xpm image
    if( draw_xpm(current_image, current_x, NEW_HS_NAME_Y, d_buf) != 0 ){
        return 1;
    }

    //increment the position
    str_pos++;

    //Just in case. Shouldn't happen, however.
    if(str_pos == HS_NAME_MAX_SIZE)
      break;

  }while(current_char != '\0');

  //update the vram's contents
  update_vram();

  return 0;
}

//Function which draws the changing objects every frame, when the player is adding
int new_highscore_loop(char* name){

  //Copy the background to the double buffer
  copy_from_background();

  //Draw the current name in the double buffer
  if( draw_new_name(name) != 0 ){
    return 1;
  }

  //update the vram
  update_vram();

  return 0;
}

//Mouse handler for the new highscore screen
void new_highscore_mouse_handler(struct packet *read_packet, char* store_in, int *str_pos){

  //if we clicked the right mouse button
  if(read_packet->rb){

      //reset variables to default
      memset(store_in, '\0', HS_NAME_MAX_SIZE);
      (*str_pos) = 0;

    //we want to discard our new highscore
    program_state = menu_open;
  }
}

//Keyboard handler for the new highscore screen
void new_highscore_kbd_handler(uint8_t scancode, char* store_in, int *str_pos){
  
  //if the pressed key is on the 1st line
  if(scancode >= MAKE_1_MIN && scancode <= MAKE_1_MAX){

    //if we have written less than 5 characters, we can still write more
    if( *str_pos < 5 ){
      
      //save the inputted character into the correct position of the string
      store_in[*str_pos] = make_line_1[scancode - MAKE_OFFSET_1];

      //increment the position
      (*str_pos)++;
    }
  }
  
  //else if the pressed key is on the 2nd line
  else if(scancode >= MAKE_2_MIN && scancode <= MAKE_2_MAX){

    //if we have written less than 5 characters, we can still write more
    if( *str_pos < 5 ){
      //save the inputted character into the correct position of the string
      store_in[*str_pos] = make_line_2[scancode - MAKE_OFFSET_2];

      //increment the position
      (*str_pos)++;
    }
  }
  
  //else if the pressed key is on the 3rd line
  else if(scancode >= MAKE_3_MIN && scancode <= MAKE_3_MAX){

    //if we have written less than 5 characters, we can still write more
    if( *str_pos < 5 ){
      //save the inputted character into the correct position of the string
      store_in[*str_pos] = make_line_3[scancode - MAKE_OFFSET_3];

      //increment the position
      (*str_pos)++;
    }
  }
  //else if the backspace was pressed
  else if(scancode == BACKSPACE_MAKE){
    
    //so long as we have already written at least 1 character into the name
    if( *str_pos > 0 ){
      
      //decrement the position
      (*str_pos)--;

      //replace the character in the str_pos position with the null character
      store_in[*str_pos] = '\0';
    }
  }
  //else if the enter key was pressed
  else if(scancode == ENTER_MAKE){

    //so long as we have already written at least 1 character into the name
    if( *str_pos > 0 ){

      //we want to save our new highscore
      program_state = save_new_hs;
    }
  }
  //else if the esc was let go of
  else if(scancode == ESC_BREAKCODE){

    //reset variables to default
    memset(store_in, '\0', HS_NAME_MAX_SIZE);
    (*str_pos) = 0;

    //we want to discard our new highscore
    program_state = menu_open;
  }
}
