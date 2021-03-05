#include "lcom/lcf.h"

#include <stdlib.h>

#include "game.h"
#include "sprite.h"     //also includes video.h
#include "logic.h"
#include "logic_defines.h"
#include "scancode_defines.h"

//extern program state variable
extern state program_state;

//holds data related to the game's logic
logic_data logic_manager;

//initializes the logic data, upon starting a new game
void initialize_logic_data(){
  logic_manager.enemy_next_horizontal = right;
  logic_manager.enemy_moves = 0;
  logic_manager.shoot_cooldown = SHOOT_COOLDOWN;
  logic_manager.enemy_move_time = START_MOVE_TIME;
  logic_manager.destr_normal = 0;
  logic_manager.player_next_horizontal = none;
  logic_manager.shoot_cooldown = SHOOT_COOLDOWN;
}

//Function which prepares the game
int prepare_game(){

  //initialize logic manager data
  initialize_logic_data();

  //generate game objects
  if( generate_game_objects() != 0 ){
    printf("Could not generate the game objects.\n");
    return 1;
  }

  //Draw the game background on the background buffer
  if( draw_game_background() != 0 ){
    printf("Could not draw the game background on the background buffer.\n");
    return 1;
  }

  return 0;
}

//Function which readies the game for a new round
int new_round(){

  //reset the game state
  if( reset_gamestate(&logic_manager) != 0 ){
    printf("Unable to reset the gamestate.\n");
    return 1;
  }
  
  //redraws the game objects
  if( draw_game_objects() != 0 ){
    printf("Unable to draw renewed objects.\n");
    return 1;
  }

  //updates the vram
  update_vram();

  //waits 1 sec because aesthetics
  sleep(1);

  return 0;
}

//Function that ends the game
void end_game(){

  //Destroys all game entities
  destroy_all_game_entities();
  
}

//Core gameplay loop
int game_loop(int tick){

  //Check if all enemies have been destroyed
  if( logic_manager.destr_normal == ENEMY_NUMBER ){

    //if that is the case, starts a new round
    if( new_round() != 0 ){
      printf("Unable to start a new round.\n");
      return 1;
    }

  }


  //reduce the cooldown on the shots
  logic_manager.shoot_cooldown--;


  //EVERY 15 FRAMES
  if(tick % 15 == 0 ){

    //move the special enemy
    move_special();

  }


  //EVERY 60 FRAMES
  if(tick % 60 == 0){

    //1/15th of a chance to generate a special enemy
    if( rand() % 15 == 0 ){

      //randomly generate a direction for the special enemy
      horizontal special_dir;
      switch (rand() % 2)
      {
      case 0:
        special_dir = left;
        break;
      case 1:
        special_dir = right;
        break;
      }

      //generate a special enemy
      if( generate_special(special_dir) != 0 ){
        return 1;
      }

    }

  }


  //EVERY enemy_move_time/2 FRAMES
  if(tick % (logic_manager.enemy_move_time / 2) == 0){
    
    //changes enemy type to be displayed
    change_enemies_animation();

  }


  //EVERY enemy_move_time FRAMES
  if(tick % logic_manager.enemy_move_time == 0){
    
    //if the enemies have moved all the way to the other side of the screen
    if(logic_manager.enemy_moves == ENEMY_MOVES) {

      //moves enemy straight down
      move_all_enemies(true, none);

      //reset the number of enemy moves to 0
      logic_manager.enemy_moves = 0;

      //if the enemies were moving right, we change their directions to the left
      if(logic_manager.enemy_next_horizontal == right){

        logic_manager.enemy_next_horizontal = left;

      }
      else{ //and vice versa

        logic_manager.enemy_next_horizontal = right;

      }
    }
    else{

      //moves enemy to the enemy_next_horizontal side
      move_all_enemies(false, logic_manager.enemy_next_horizontal);

      //increment the number of enemy moves
      logic_manager.enemy_moves++;

    }

    //check if any enemy object collided with the player, after they move
    program_state = check_player_enemy_col();

    //check if the game has ended
    if( program_state == game_over ){
      return 0;
    }

  }


  //EVERY 5 FRAMES
  if(tick % 5 == 0){

    //if not all enemies are yet destroyed
    if( logic_manager.destr_normal < ENEMY_NUMBER ){

      //1/3rd of a chance to spawn an enemy projectile
      if( rand() % 3 == 0){

        //pick one random enemy to spawn the object from
        int spawner = rand() % 66;

        //generate one enemy projectile
        if( add_enemy_projectile(spawner) != 0 ){
          return 1;
        }

      }

    }

    //move all projectiles
    move_all_projectiles();

    //increment the number of enemies destroyed
    unsigned int destroyed = projectile_col();
    logic_manager.destr_normal += destroyed;

    //if we have destroyed a multiple of 11 normal enemies (excluding 0),
    //decrease the time between movements (as long as it's higher than 15 ticks)
    if( logic_manager.destr_normal % 11 == 0 && destroyed != 0 && logic_manager.enemy_move_time > 15){
      logic_manager.enemy_move_time -= 15;
    }

  }


  //EVERY FRAME
  //check if the player has lost all hp
  program_state = check_player_hp();
  
  //check if the game has ended
  if( program_state == game_over ){
    return 0;
  }
  
  //draw game objects on the double buffer
  if( draw_game_objects() != 0 ) { return 1; }
  
  //update the vram
  update_vram();

  return 0;
}

//Function which handles the data received from the keyboard interrupts, during the game
void game_kbd_handler(uint8_t scancode){
  switch (scancode)
  {

    //if the player pressed the A key
    case A_MAKECODE:

      //the player will move left next
      logic_manager.player_next_horizontal = left;

      //we move the player to that direction
      move_player(logic_manager.player_next_horizontal);

      break;


    //if the player pressed the D key
    case D_MAKECODE:

      //the player will move left next
      logic_manager.player_next_horizontal = right;

      //we move the player to that direction
      move_player(logic_manager.player_next_horizontal);

      break;

      
    //if the player pressed the esc key
    case ESC_BREAKCODE:

      //change the state to the appropriate one
      program_state = game_over;

      break;


    default:
      break;
  }
}

//Function which handles the data received from the mouse interrupts, during the game
int game_mouse_handler(struct packet *read_packet){

  //if we clicked the left mouse button
  if(read_packet->lb){

    //if the player shot is not on cooldown
    if( logic_manager.shoot_cooldown <= 0 ){

      //try to shoot
      if(shoot() != 0){
        printf("Error shooting.\n");
        return 1;
      }

      //reset shot cooldown
      logic_manager.shoot_cooldown = SHOOT_COOLDOWN;

    }

  }
  //if we clicked the right mouse button
  else if(read_packet->rb){

    //change the state to the appropriate one
    program_state = game_over;

  }

  return 0;
}
