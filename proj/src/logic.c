#include "lcom/lcf.h"

#include <math.h>
#include "logic.h"
#include "sprite.h"
#include "menu.h"
#include "video_defines.h"
#include "logic_defines.h"

//Images to be used
extern struct Image game_img[6];
extern struct Image enemy_img[7];

//Enemy vector, and player object
Cursor* cursor_obj = NULL;
Enemy* enemies[ENEMY_NUMBER] = {NULL};
Enemy* special = NULL;
Player* player_obj = NULL;
Player_projectile* player_projectiles[MAX_PLAYER_PROJ] = {NULL};
Enemy_projectile* enemy_projectiles[MAX_ENEMY_PROJ] = {NULL};

/*
        CURSOR GENERATION FUNCTION
*/
//Function which generates the cursor object
int generate_cursor(){

    //allocate memory for the cursor object
    cursor_obj = (Cursor *) malloc(sizeof(cursor_obj));

    //check if we could not successfully alocate the memory
    if(cursor_obj == NULL){
        printf("Failed creating cursor.\n");
        return 1;
    }

    //initialize the cursor's image
    cursor_obj -> img = &game_img[crosshair_img];

    //initialize the cursor's position values (middle of the screen)
    cursor_obj -> x = SCREEN_RES_X / 2;
    cursor_obj -> y = SCREEN_RES_Y / 2;

    return 0;
}

/*
            GAME OBJECT GENERATION FUNCTIONS
*/

//Function which generates the player object
int generate_player(){

    //allocate memory for the player object
    player_obj = (Player *) malloc(sizeof(Player));

    //check if we could not successfully alocate the memory
    if(player_obj == NULL){
        printf("Failed creating player.\n");
        return 1;
    }

    //initialize player's live number
    player_obj -> hp = INIT_PLAYER_LIVES;
    player_obj -> points = 0;

    //initialize player's image
    player_obj -> img = &game_img[player_img];

    //initialize player's width and height (from the image)
    player_obj -> width = game_img[player_img].image_info.width;
    player_obj -> height = game_img[player_img].image_info.height;

    //initialize player's position
    // x - middle of the screen ( half of the player's width to the left, so he's centered)
    // y - specified position (CHANGE LATER)
    player_obj -> x = (SCREEN_RES_X / 2) - (player_obj->width / 2);
    unsigned int y_offset = 3 * player_obj->height + (player_obj->height / 2);
    player_obj -> y = SCREEN_RES_Y - y_offset;

    //initialize the position we're shooting our objects from
    // x -> middle of the player
    // y -> top of the player (player's y position - 1)
    player_obj -> shoot_y = player_obj -> y - 1;
    player_obj -> shoot_x = player_obj -> x + ((player_obj -> width) / 2) - 1;

    //initialize player's speed
    player_obj -> speed = PLAYER_SPEED;

  return 0;
}

//Function which creates an enemy object
Enemy *create_enemy(bool is_enemy_1, uint16_t start_x, uint16_t start_y, uint16_t vx, uint16_t vy){
    
    //allocate memory for the enemy object
    Enemy *to_return = (Enemy *) malloc(sizeof(Enemy));
    
    //check if we could not successfully alocate the memory
    if( to_return == NULL ){
        return NULL;
    }

    //check enemy type, and retrieve the appropriate images
    if(is_enemy_1){
        to_return -> img_1 = &enemy_img[type1_A];
        to_return -> img_2 = &enemy_img[type1_B];
        to_return -> value = ENEMY_1_VALUE;
    }
    else{
        to_return -> img_1 = &enemy_img[type2_A];
        to_return -> img_2 = &enemy_img[type2_B];
        to_return -> value = ENEMY_2_VALUE;
    }

    //initialize the enemy's height and width (from the image)
    to_return -> width = to_return -> img_1 -> image_info.width;
    to_return -> height = to_return -> img_1 -> image_info.height;

    //initialize the enemy's x position (passed as a parameter)
    to_return -> x = start_x;
    to_return -> y = start_y;

    //initialize the enemy's velocities
    to_return -> x_speed = vx;
    to_return -> y_speed = vy;

    //we draw version 1 of the enemy first
    to_return -> version1 = true;

    return to_return;
}

//Function which loads all enemy objects
int generate_enemies(){

    //y value each sprite will start at
    uint16_t starting_y = ENEMY_START_Y;
    uint16_t starting_x = ENEMY_START_X;

    for( unsigned int e = 0; e < ENEMY_NUMBER; e++){

        bool enemy_1;

        //if our next enemy is the first on the next line (not counting enemy 0)
        if( ((e % ENEMIES_PER_LINE) == 0)  && (e != 0)){
            starting_y += 48;
            starting_x = ENEMY_START_X;
        }

        //if the enemies are on an even numbered row, we create an enemy of type 1
        if((e / ENEMIES_PER_LINE) % 2 == 0 ){
            enemy_1 = true;
        }
        else{   //else, we create an enemy of type 2
            enemy_1 = false;
        }

        //create a pointer to the enemy
        enemies[e] = create_enemy(enemy_1, starting_x, starting_y, ENEMY_X_SPEED, ENEMY_Y_SPEED);

        //check if we were sucessfull in doing so (we are unable to start the game, if that's the case)
        if(enemies[e] == NULL){
            printf("Failed creating enemy number %d.\n", e);
            return 1;
        }

        //get the next enemy's starting position
        starting_x += 64;
    }

    return 0;
}

//Function which generates all objects AT THE START OF THE GAME
int generate_game_objects(){

    //generate player
    if( generate_player() != 0 ){
        printf("Failed generating the player object.\n");
        return 1;
    }

    //generate enemies
    if( generate_enemies() != 0 ){
        printf("Failed generating the enemy objects.\n");
        return 1;
    }

    return 0;
}

//Generates a special enemy
int generate_special(horizontal direction){

    //If no special enemy exists
    if( special == NULL ){
        
        //allocate memory for the enemy object
        special = (Enemy *) malloc(sizeof(Enemy));
        
        //check if we could not successfully alocate the memory
        if( special == NULL ){
            return 1;
        }

        special -> img_1 = &enemy_img[special_A];
        special -> img_2 = &enemy_img[special_B];
        special -> value = SPECIAL_VALUE;

        special -> version1 = true;

        //initialize the enemy's height and width (from the image)
        special -> width = special -> img_1 -> image_info.width;
        special -> height = special -> img_1 -> image_info.height;

        //initialize the directiton, positions and speed
        special -> direction = direction;
        special -> y = special -> height / 2;
        switch(direction){
            //if the enemy is moving left
            case left:
                //the initial position is just outside of the screen bounds
                special -> x = SCREEN_RES_X;
                special -> x_speed = 2 * ENEMY_X_SPEED;
                break;
            //if the enemy is moving right
            case right:
                //the initial position is outside of the screen bounds to the left
                special -> x = MIN_X_PIXEL - special -> width;
                special -> x_speed = 2 * ENEMY_X_SPEED;
                break;
            //this won't happen
            default:
                break;
        }
        
        //initialize direction and speed
        return 0;

    }
    //there's already an enemy, we just exit, no cause for concern
    return 0;
}

//Generates a line based on the first and last points of a line
//code apropriated from http://members.chello.at/~easyfilter/bresenham.html
int *bresenham_line_algorithm(int x0, int y0, int xf, int yf){

    int dx =  abs(xf-x0);
    int sx = x0<xf ? 1 : -1;
    int dy = -abs(yf-y0);
    int sy = y0<yf ? 1 : -1; 
    int err = dx+dy, e2; /* error value e_xy */

    //alocate memory for the points of the line (2 times the number of points)
    int *to_return = (int *) malloc(2*PROJECTILE_SIZE*sizeof(int));

    //loop to generate PROJECTILE_SIZE number of points
    for( unsigned int i = 0; i < PROJECTILE_SIZE; i++){

        //initialize the x and y value of the projectile's point number i
        to_return[2*i] = x0;
        to_return[2*i + 1] = y0;

        e2 = 2*err;

        if (e2 >= dy) {
            err += dy; x0 += sx;
        } /* e_xy+e_x > 0 */

        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        } /* e_xy+e_y < 0 */
    }

    return to_return;
}

//Creates a projectile object
Player_projectile *create_player_projectile(int16_t mouse_x, int16_t mouse_y){

    //Alocates memory for the projectile
    Player_projectile *to_return = (Player_projectile *) malloc(sizeof(Player_projectile));
    
    //check if we could not successfully alocate the memory
    if(to_return == NULL){
        return NULL;
    }

    //Generates the projectile's line
    to_return -> line = bresenham_line_algorithm(player_obj -> shoot_x, player_obj -> shoot_y, mouse_x, mouse_y);

    //calculate angle
    double x0 = to_return->line[0];
    double y0 = to_return->line[1];
    double xf = to_return->line[PROJECTILE_SIZE * 2 - 2];
    double yf = to_return->line[PROJECTILE_SIZE * 2 - 1];
    double dx = (xf-x0);
    double dy = (yf-y0);
    double theta = atan(dy/dx);

    //When dx < 0, our angle is obligatorily higher than 90º and -90º < atan < 90º
    //Therefore, we must add Pi to our calculated value
    if( dx < 0 ){
        theta = theta + M_PI;
    }

    //calculate vx and vy for the projectile (vx = v*cos(theta), vy = v*sin(theta))
    to_return -> vx = (int) floor((double) PROJECTILE_SPEED * cos(theta));
    to_return -> vy = (int) floor((double) PROJECTILE_SPEED * sin(theta));

    return to_return;
}

//Adds a new projectile object to the player_projectiles array
int add_player_projectile(int16_t mouse_x, int16_t mouse_y){
    
    //iterate throught the player_projectiles vector,
    //in order to find a slot to add the projectile to
    for( unsigned int p = 0; p < MAX_PLAYER_PROJ; p++){

        //If no projectile is saved in that slot
        if( player_projectiles[p] == NULL ){

            //creates the projectile we're adding to the array
            Player_projectile *to_add = create_player_projectile(mouse_x, mouse_y);

            //check if we could not successfully alocate the memory
            if(to_add == NULL){
                return 1;
            }

            //add the projectile to the array
            player_projectiles[p] = to_add;
            return 0;
        }
    }
    
    //if there's no projectile slot available, we do nothing
    return 0;
}

//Shoots towards the current mouse coordinates
int shoot(){
    return add_player_projectile(cursor_obj -> x, cursor_obj -> y);
}

//Adds a new Enemy_projecctile object to the enemy_projectiles
int add_enemy_projectile(int spawner_index){

    //get one enemy object that exists
    int enemy_no = spawner_index;
    while( enemies[enemy_no] == NULL ){
        //get the next enemy position
        enemy_no++;
        //make sure it's between 0 and ENEMY_NUMBER-1
        enemy_no = enemy_no % ENEMY_NUMBER;
    }

    //iterate throught the enemy_projectiles vector,
    //in order to find a slot to add the projectile to
    for( unsigned int p = 0; p < MAX_ENEMY_PROJ; p++){

        //If no projectile is saved in that slot
        if( enemy_projectiles[p] == NULL ){

            //allocate memory for the enemy projectile object
            Enemy_projectile* to_add = (Enemy_projectile *) malloc(sizeof(Enemy_projectile));

            //check if we could not successfully alocate the memory
            if(to_add == NULL){
                return 1;
            }

            //Initialize values
            to_add -> x = enemies[enemy_no] -> x + (enemies[enemy_no] -> width / 2);
            to_add -> y = enemies[enemy_no] -> y + enemies[enemy_no] -> height;
            to_add -> speed = PROJECTILE_SPEED;
            to_add -> img = &enemy_img[enemy_proj];

            to_add -> width = to_add -> img -> image_info.width;
            to_add -> height = to_add -> img -> image_info.height;

            //add the projectile to the array
            enemy_projectiles[p] = to_add;
            return 0;
        }
    }
    
    //if there's no projectile slot available, we do nothing
    return 0;
}

/*
                MENU INTERACTION FUNCTIONS
*/

state menu_option(MenuItems *buttons){
    //check if the play button was clicked
    int play_x0 = buttons -> play_button_x;
    int play_y0 = buttons -> play_button_y;
    int play_xf = play_x0 + buttons -> play_button_width;
    int play_yf = play_y0 + buttons -> play_button_height;
    if( (cursor_obj -> x) > play_x0 && (cursor_obj -> x) < play_xf ){
        if( (cursor_obj -> y) > play_y0 && (cursor_obj -> y) < play_yf ){
            return game_start;
        }
    }
    
    //check if the highscores button was clicked
    int highscore_x0 = buttons -> highscore_x;
    int highscore_y0 = buttons -> highscore_y;
    int highscore_xf = highscore_x0 + buttons -> highscore_width;
    int highscore_yf = highscore_y0 + buttons -> highscore_height;
    if( (cursor_obj -> x) > highscore_x0 && (cursor_obj -> x) < highscore_xf ){
        if( (cursor_obj -> y) > highscore_y0 && (cursor_obj -> y) < highscore_yf ){
            return highscore_open;
        }
    }
    //if no relevant button was clicked, we stay in the menu
    return menu;
}

//Function which checks if the player has achieved a new highscore, and saves it, if that's the case
int check_new_highscore(int *s){

    //the arrays are sorted, so the last score is the smallest
    //if the player's score is higher than the last item in the array
    if( (int) player_obj->points > s[MAX_HIGHSCORES - 1] ){
        return player_obj->points;
    }

    //if our score is smaller than every other score on the array
    return 0;
    
}

/*
              GAME MOVEMENT LOGIC FUNCTIONS
*/

//Function that moves the player
void move_player(horizontal direction){

    //if we're at the left side of the screen and want to move left,
    //or at the right side of the screen and want to move right
    if( (player_obj->x == MIN_X_PIXEL && direction == left) || (player_obj->x == (int)(SCREEN_RES_X - player_obj->width) && direction == right)){
        return;
    }

    //increment the player's position
    player_obj->x += (player_obj->speed * direction);

    //increment the "cannon"'s position
    player_obj->shoot_x = player_obj->x + (player_obj->width / 2) - 1;
}

//Function which updates the cursor position
void move_cursor(int16_t dx, int16_t dy){
    //dx is positive rightwards, and dy is positive upwards
    //so we add the x value and subtract the y value
    int16_t new_x = cursor_obj -> x + dx;
    int16_t new_y = cursor_obj -> y - dy;

    //if it's out of bounds on the x axis
    if(new_x < MIN_X_PIXEL){        //to the left of the screen
        new_x = MIN_X_PIXEL;
    }
    else if(new_x > MAX_X_PIXEL)    //to the right of the screen
    {
        new_x = MAX_X_PIXEL;
    }

    //if it's out of bounds on the y axis
    if(new_y < MIN_Y_PIXEL){        //to the left of the screen
        new_y = MIN_Y_PIXEL;
    }
    else if(new_y > MAX_Y_PIXEL)    //to the right of the screen
    {
        new_y = MAX_Y_PIXEL;
    }
    
    //update the cursor values
    cursor_obj -> x = new_x;
    cursor_obj -> y = new_y;
}

//Function that moves an enemy
void move_enemy(Enemy *to_move, bool move_down, horizontal direction){
    if(move_down){
        to_move->y += to_move->y_speed;
    }

    to_move->x += (to_move->x_speed * direction);    
}

//Function which moves ALL enemies
void move_all_enemies(bool down, horizontal direction){

    for( unsigned int e = 0; e < ENEMY_NUMBER; e++){
        if(enemies[e] == NULL){
            continue;
        }
        move_enemy(enemies[e], down, direction);
    }
}

//Function which moves a special enemy
void move_special(){

    //if the special object exists
    if( special != NULL ){

        //moves the special enemy
        move_enemy(special, false, special -> direction);
        special->version1 = !special->version1;

        //check destruction of the object, which depends on the direction it's traveling
        switch(special -> direction){
            case left:
                //if the object has left the screen bounds to the left
                if( special->x <= (-special->width)){
                    destroy_special();
                }
                break;
            case right:
                //if the object has left the screen bounds to the right
                if( special->x >= SCREEN_RES_X + special->width){
                    destroy_special();
                }
                break;
            case none:
                break;
            default:
                break;
        }
    }
}

//Function which moves a player projectile
void move_player_projectile(Player_projectile *to_move){
    //Iterate through the points of the projectile
    for( unsigned int p = 0; p < PROJECTILE_SIZE; p++){

        //get the speeds on the x and y direction
        int vx = to_move->vx;
        int vy = to_move->vy;

        //increment x and y values by the respective speeds
        to_move->line[2*p]     += vx;
        to_move->line[2*p + 1] += vy;
    }
}

//Function which moves a enemy projectile
void move_enemy_projectile(Enemy_projectile *to_move){
    
    //Iterate through the points of the projectile
    to_move -> y += to_move -> speed;
}

//Function which moves ALL player_projectiles and enemy_projectiles
void move_all_projectiles(){

    //Iterate through the player_projectiles objects
    for( unsigned int p = 0; p < MAX_PLAYER_PROJ; p++){
        //Check if our projectile is a NULL pointer (does not exist)
        if(player_projectiles[p] != NULL){
            //Move the valid projectile
            move_player_projectile(player_projectiles[p]);
        }
    }

    //Iterate through the enemy_projectiles objects
    for( unsigned int p = 0; p < MAX_ENEMY_PROJ; p++){
        //Check if our projectile is a NULL pointer (does not exist)
        if(enemy_projectiles[p] != NULL){
            //Move the valid projectile
            move_enemy_projectile(enemy_projectiles[p]);
        }
    }
}

/*
               GAME COLLISION AND OBJECT DESTRUCTION LOGIC FUNCTIONS
*/

//Function which destroys one enemy object
void destroy_enemy(int index){
    //free the alocated memory
    free(enemies[index]);
    //change the pointer value to NULL
    enemies[index] = NULL;
}

//Function which destroys the special enemy object
void destroy_special(){
    //free the alocated memory
    free(special);
    //change the pointer value to NULL
    special = NULL;
}

//Function which destroys the player object
void destroy_player(){
    //free the alocated memory
    free(player_obj);
    //change the pointer value to NULL
    player_obj = NULL;
}
//Function which destroys one player projectile object
void destroy_player_projectile(int index){
    //free the alocated memory
    free(player_projectiles[index]);
    //change the pointer value to NULL
    player_projectiles[index] = NULL;
}

//Function which destroys one enemy projectile object
void destroy_enemy_projectile(int index){
    //free the alocated memory
    free(enemy_projectiles[index]);
    //change the pointer value to NULL
    enemy_projectiles[index] = NULL;
}

//Function which destroys ALL projectiles, both the player's and the enemies'
void destroy_all_projectiles(){
    for( unsigned int p = 0; p < MAX_PLAYER_PROJ; p++){
        
        //reference to the current projectile
        Player_projectile *current_projectile = player_projectiles[p];

        //if our current projectile does not exist
        if( current_projectile == NULL ){
            continue;
        }

        destroy_player_projectile(p);
    }

    for( unsigned int p = 0; p < MAX_ENEMY_PROJ; p++){

        //reference to the current projectile
        Enemy_projectile * current_projectile = enemy_projectiles[p];

        //if our current projectile does not exist
        if( current_projectile == NULL ){
            continue;
        }

        destroy_enemy_projectile(p);
    }
}

//Function which destroys ALL game entities used by the game
void destroy_all_game_entities(){

    //destroys all projectiles
    destroy_all_projectiles();

    //destroys all normal enemies, if they exist
    for(unsigned int e = 0; e < ENEMY_NUMBER; e++){
        if( enemies[e] != NULL ){
            destroy_enemy(e);
        }
    }

    //destroys special enemy, if it exists
    if( special != NULL ){
        destroy_special();
    }

    if( player_obj != NULL ){
        destroy_player();
    }

}
//Checks for colisions between the player projectiles and the enemy objects and destroys 
//the objects that collided, and between the enemy projectiles and the player
unsigned int projectile_col(){

    //Number of destroyed normal enemies (to return later)
    unsigned int destroyed_normal_enemies = 0;

    //CHECK PLAYER PROJECTILE COLISIONS
    //iterate through the projectiles
    for( unsigned int p = 0; p < MAX_PLAYER_PROJ; p++){

        //if this projectile does not exist
        if(player_projectiles[p] == NULL){
            continue;
        }

        //retrieves the projectile's final point's coordinates
        unsigned int proj_xf = player_projectiles[p]->line[PROJECTILE_SIZE * 2 - 2];
        unsigned int proj_yf = player_projectiles[p]->line[PROJECTILE_SIZE * 2 - 1];

        //If the last point of the projectile touched one of the limits of the screen
        if(proj_xf <= MIN_X_PIXEL || proj_xf >= MAX_X_PIXEL || proj_yf <= MIN_Y_PIXEL || proj_yf >= MAX_Y_PIXEL - UI_BAR_HEIGHT){
            //destroy the projectile
            destroy_player_projectile(p);
            continue;
        }

        //iterate through the enemies, to check colisions with them
        for( unsigned int e = 0; e < ENEMY_NUMBER; e++){
            
            //if this enemy does not exist
            if(enemies[e] == NULL){
                continue;
            }

            //get the projectile's first and final points, to implement
            //AABB colision detection
            unsigned int enemy_x0 = enemies[e]->x;
            unsigned int enemy_y0 = enemies[e]->y;
            unsigned int enemy_xf = enemy_x0 + enemies[e]->width;
            unsigned int enemy_yf = enemy_y0 + enemies[e]->height;  

            //if the last point of the projectile is inside the bounds
            //of the enemy, a colision occurred
            if(proj_xf > enemy_x0 && proj_xf < enemy_xf){       //check x bounds
                if(proj_yf > enemy_y0 && proj_yf < enemy_yf){   //check y bounds

                    //increment the player's points with the enemy's value
                    player_obj -> points += enemies[e] -> value;
                    destroyed_normal_enemies++;
                    //destroy the projectile that collided
                    destroy_player_projectile(p);
                    //destroy the enemy
                    destroy_enemy(e);
                    break;
                }
            }
        }

        //if the special object has been spawned
        if( special != NULL ){

            //get its initial and final points
            unsigned int special_x0 = special->x;
            unsigned int special_y0 = special->y;
            unsigned int special_xf = special_x0 + special->width;
            unsigned int special_yf = special_y0 + special->height;

            if(proj_xf > special_x0 && proj_xf < special_xf){       //check x bounds
                if(proj_yf > special_y0 && proj_yf < special_yf){   //check y bounds

                    //increment the player's points with the special enemy's value
                    player_obj -> points += special -> value;
                    //destroy the projectile that collided
                    destroy_player_projectile(p);
                    //destroy the special enemy
                    destroy_special();
                }
            }   
        }
    }

    //points cap out at 999999
    if(player_obj -> points >= 99999){
        player_obj -> points = 99999;
    }

    //CHECK ENEMY PROJECTILE COLISIONS
    //Iterate through the enemy_projectiles objects
    for( unsigned int p = 0; p < MAX_ENEMY_PROJ; p++){

        //reference to the current enemy_projectile
        Enemy_projectile* current_enemy_projectile = enemy_projectiles[p];

        //Check if our projectile is a NULL pointer (does not exist)
        if(current_enemy_projectile == NULL){
            continue;
        }

        //get projectile's first and final point's coordinates
        unsigned int proj_x0 = current_enemy_projectile -> x;
        unsigned int proj_xf = proj_x0 + current_enemy_projectile -> width;
        unsigned int proj_y0 = current_enemy_projectile -> y;
        unsigned int proj_yf = proj_y0 + current_enemy_projectile -> height;
            
        //If the projectile touches the UI bar
        unsigned int ui_first_pixel = MAX_Y_PIXEL - UI_BAR_HEIGHT;
        if( proj_yf >= ui_first_pixel){
            destroy_enemy_projectile(p);
            continue;
        }
        

        //get the players first and final points, to implement
        //AABB colision detection
        unsigned int player_x0 = player_obj->x;
        unsigned int player_y0 = player_obj->y;
        unsigned int player_xf = player_x0 + player_obj->width;
        unsigned int player_yf = player_y0 + player_obj->height;

        if( (proj_x0 > player_x0 && proj_x0 < player_xf) || (proj_xf > player_x0 && proj_xf < player_xf) ){     //check x bounds
            if( (proj_y0 > player_y0 && proj_y0 < player_yf) || (proj_yf > player_y0 && proj_yf < player_yf)){  //check y bounds
                if( player_obj -> hp > 0 )
                    player_obj -> hp--;
                
                destroy_enemy_projectile(p);
                continue;
            }
        }

    }

    return destroyed_normal_enemies;
}

//Function which checks for conditions that end the game on a player loss (colisions between the
//player and the enemies, the enemies and the ground, and if the player's hp drops to 0)
state check_player_enemy_col(){

    //CHECK IF PLAYER'S HP IS 0
    if( player_obj -> hp == 0 ){
        return game_over;
    }

    //CHECK PLAYER COLLISION WITH AN ENEMY OBJECT
    //get the player's first and final points, to implement
    //AABB colision detection
    unsigned int player_x0 = player_obj->x;
    unsigned int player_y0 = player_obj->y;
    unsigned int player_xf = player_x0 + player_obj->width;
    unsigned int player_yf = player_y0 + player_obj->height;

    //iterate through the enemies
    for( unsigned int e = 0; e < ENEMY_NUMBER; e++){

        //reference to the current enemy
        Enemy *current_enemy = enemies[e];

        //if this enemy does not exist
        if(current_enemy == NULL){
            continue;
        }

        //get the current enemy's first and final points
        unsigned int enemy_x0 = current_enemy->x;
        unsigned int enemy_y0 = current_enemy->y;
        unsigned int enemy_xf = enemy_x0 + current_enemy->width;
        unsigned int enemy_yf = enemy_y0 + current_enemy->height;

        //if the enemy's final y touched or went past to the ground (line above the ui bar)
        if( enemy_yf >= (SCREEN_RES_Y - UI_BAR_HEIGHT) ){
            return game_over;
        }


        //if the last point of the enemy is inside the bounds
        //of the player, a colision occurred
        if( (enemy_y0 > player_y0 && enemy_y0 < player_yf) || (enemy_yf > player_y0 && enemy_yf < player_yf) ){       //check y bounds
            if( (enemy_x0 > player_x0 && enemy_x0 < player_xf) || (enemy_xf > player_x0 && enemy_xf < player_xf) ){   //check x bounds
                return game_over;
            }
        }
    }
    return game_ok;
}

//Function that checks if the player's hp dropped to 0
state check_player_hp(){
    //if the player's hp is above 1, the game can continue as normal
    if( player_obj->hp > 0 ){
        return game_ok;
    }
    return game_over;
}

//Function that resets the game at the start of a new round
state reset_gamestate(logic_data *to_update){

    //Destroy all projectiles currently in game
    destroy_all_projectiles();

    //Increment the player's hp as a reward for beating the game once
    player_obj -> hp++;

    //Update our logic_manager
    to_update->enemy_next_horizontal = right;
    to_update->enemy_moves = 0;
    to_update->enemy_move_time = START_MOVE_TIME;
    to_update->destr_normal = 0;

    //Initializes the enemy array once more
    if( generate_enemies() != 0 ){
        return 1;
    }

    return 0;
}
