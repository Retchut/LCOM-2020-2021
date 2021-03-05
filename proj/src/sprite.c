#include "lcom/lcf.h"

#include <ctype.h>

#include "sprite.h"
#include "logic.h"
#include "video_defines.h"
#include "logic_defines.h"
#include "xpm_files.h"
#include "menu.h"

//Objects
extern unsigned int bytes_per_pixel;
extern Enemy* enemies[ENEMY_NUMBER];
extern Enemy* special;
extern Player* player_obj;
extern Cursor* cursor_obj;
extern Player_projectile* player_projectiles[];
extern Enemy_projectile* enemy_projectiles[];
extern MenuItems* menu_obj;

//Game Images to be used
Image menu_img[7];
Image game_img[3];
Image enemy_img[7];

//Number images (in the format: {n0, n1, n2....., n9})
//Image numbers[10];
Image numbers[11];

//Letter images (int the format: {a, b, c, d, ...., z})
Image letters[26];



/*
            GENERIC IMAGE LOADING AND DRAWING FUNCTIONS
*/


//Function which loads all images later used by the game
void load_images(){
    //Load menu sprites
    menu_img[menu_back].color_map = xpm_load(menu_background, XPM_8_8_8, &(menu_img[menu_back].image_info));
    menu_img[title_1].color_map = xpm_load(menu1, XPM_8_8_8, &(menu_img[title_1].image_info));
    menu_img[title_2].color_map = xpm_load(menu2, XPM_8_8_8, &(menu_img[title_2].image_info));
    menu_img[p_butt].color_map = xpm_load(play_button, XPM_8_8_8, &(menu_img[p_butt].image_info));
    menu_img[h_butt].color_map = xpm_load(highscore_button, XPM_8_8_8, &(menu_img[h_butt].image_info));
    menu_img[high_back].color_map = xpm_load(highscore_background, XPM_8_8_8, &(menu_img[high_back].image_info));
    menu_img[new_high].color_map = xpm_load(new_highscore, XPM_8_8_8, &(menu_img[new_high].image_info));

    //Load game sprites
    game_img[game_back].color_map = xpm_load(background, XPM_8_8_8, &(game_img[game_back].image_info));
    game_img[player_img].color_map     = xpm_load(player,     XPM_8_8_8, &(game_img[player_img].image_info));
    game_img[crosshair_img].color_map  = xpm_load(crosshair,   XPM_8_8_8, &(game_img[crosshair_img].image_info));
    
    //Load enemy sprites
    enemy_img[type1_A].color_map = xpm_load(enemy1_A, XPM_8_8_8, &(enemy_img[type1_A].image_info));
    enemy_img[type1_B].color_map = xpm_load(enemy1_B, XPM_8_8_8, &(enemy_img[type1_B].image_info));
    enemy_img[type2_A].color_map = xpm_load(enemy2_A, XPM_8_8_8, &(enemy_img[type2_A].image_info));
    enemy_img[type2_B].color_map = xpm_load(enemy2_B, XPM_8_8_8, &(enemy_img[type2_B].image_info));
    enemy_img[special_A].color_map = xpm_load(bonus_enemy_A, XPM_8_8_8, &(enemy_img[special_A].image_info));
    enemy_img[special_B].color_map = xpm_load(bonus_enemy_B, XPM_8_8_8, &(enemy_img[special_B].image_info));
    enemy_img[enemy_proj].color_map = xpm_load(enemy_projectile, XPM_8_8_8, &(enemy_img[enemy_proj].image_info));

    //Load number sprites and the double dots image
    numbers[0].color_map = xpm_load(n0, XPM_8_8_8, &(numbers[0].image_info));
    numbers[1].color_map = xpm_load(n1, XPM_8_8_8, &(numbers[1].image_info));
    numbers[2].color_map = xpm_load(n2, XPM_8_8_8, &(numbers[2].image_info));
    numbers[3].color_map = xpm_load(n3, XPM_8_8_8, &(numbers[3].image_info));
    numbers[4].color_map = xpm_load(n4, XPM_8_8_8, &(numbers[4].image_info));
    numbers[5].color_map = xpm_load(n5, XPM_8_8_8, &(numbers[5].image_info));
    numbers[6].color_map = xpm_load(n6, XPM_8_8_8, &(numbers[6].image_info));
    numbers[7].color_map = xpm_load(n7, XPM_8_8_8, &(numbers[7].image_info));
    numbers[8].color_map = xpm_load(n8, XPM_8_8_8, &(numbers[8].image_info));
    numbers[9].color_map = xpm_load(n9, XPM_8_8_8, &(numbers[9].image_info));
    numbers[10].color_map = xpm_load(dots, XPM_8_8_8, &(numbers[10].image_info));

    //load letter sprites
    letters[letter_a].color_map = xpm_load(la, XPM_8_8_8, &(letters[letter_a].image_info));
    letters[letter_b].color_map = xpm_load(lb, XPM_8_8_8, &(letters[letter_b].image_info));
    letters[letter_c].color_map = xpm_load(lc, XPM_8_8_8, &(letters[letter_c].image_info));
    letters[letter_d].color_map = xpm_load(ld, XPM_8_8_8, &(letters[letter_d].image_info));
    letters[letter_e].color_map = xpm_load(le, XPM_8_8_8, &(letters[letter_e].image_info));
    letters[letter_f].color_map = xpm_load(lf, XPM_8_8_8, &(letters[letter_f].image_info));
    letters[letter_g].color_map = xpm_load(lg, XPM_8_8_8, &(letters[letter_g].image_info));
    letters[letter_h].color_map = xpm_load(lh, XPM_8_8_8, &(letters[letter_h].image_info));
    letters[letter_i].color_map = xpm_load(li, XPM_8_8_8, &(letters[letter_i].image_info));
    letters[letter_j].color_map = xpm_load(lj, XPM_8_8_8, &(letters[letter_j].image_info));
    letters[letter_k].color_map = xpm_load(lk, XPM_8_8_8, &(letters[letter_k].image_info));
    letters[letter_l].color_map = xpm_load(ll, XPM_8_8_8, &(letters[letter_l].image_info));
    letters[letter_m].color_map = xpm_load(lm, XPM_8_8_8, &(letters[letter_m].image_info));
    letters[letter_n].color_map = xpm_load(ln, XPM_8_8_8, &(letters[letter_n].image_info));
    letters[letter_o].color_map = xpm_load(lo, XPM_8_8_8, &(letters[letter_o].image_info));
    letters[letter_p].color_map = xpm_load(lp, XPM_8_8_8, &(letters[letter_p].image_info));
    letters[letter_q].color_map = xpm_load(lq, XPM_8_8_8, &(letters[letter_q].image_info));
    letters[letter_r].color_map = xpm_load(lr, XPM_8_8_8, &(letters[letter_r].image_info));
    letters[letter_s].color_map = xpm_load(ls, XPM_8_8_8, &(letters[letter_s].image_info));
    letters[letter_t].color_map = xpm_load(lt, XPM_8_8_8, &(letters[letter_t].image_info));
    letters[letter_u].color_map = xpm_load(lu, XPM_8_8_8, &(letters[letter_u].image_info));
    letters[letter_v].color_map = xpm_load(lv, XPM_8_8_8, &(letters[letter_v].image_info));
    letters[letter_w].color_map = xpm_load(lw, XPM_8_8_8, &(letters[letter_w].image_info));
    letters[letter_x].color_map = xpm_load(lx, XPM_8_8_8, &(letters[letter_x].image_info));
    letters[letter_y].color_map = xpm_load(ly, XPM_8_8_8, &(letters[letter_y].image_info));
    letters[letter_z].color_map = xpm_load(lz, XPM_8_8_8, &(letters[letter_z].image_info));
}

//Function which draws one xpm image on our buffer of choice
int draw_xpm(Image *image, uint16_t x_pos, uint16_t y_pos, enum buffer to_draw_in){

    //retrieve the image info and color maps of the image to draw
    xpm_image_t *img_info = &(image->image_info);
    uint8_t *img_color_map = image->color_map;

    //number of pixels per line on the img (number of total columns)
    uint16_t x_pixels = img_info->width;
    //number of pixels per column on the img (number of total lines)
    uint16_t y_pixels = img_info->height;
    

    //iterate through the lines
    for( unsigned int line = 0; line < y_pixels; line++){

        //iterate through each column on one line
        for( unsigned int col = 0; col < x_pixels; col++){

            uint32_t offset = ((line * x_pixels) + col) * bytes_per_pixel;

            //read the current color
            uint8_t current_component = 0;
            uint32_t color = 0;
            for( unsigned int byte = 0; byte < bytes_per_pixel; byte++){
                current_component = *(img_color_map + offset + byte);  //gets the current component
                color |= (current_component << 8 * byte);               //adds it to the color
            }

            //draw the pixel at that location
            if( draw_pixel(x_pos + col, y_pos + line, color, to_draw_in) != 0 ){
                printf("Error printing the xpm image.\n");
                return 1;
            }
        }
    }

    return 0;
}



/*
            CURSOR DRAWING FUNCTION
*/


//Function which draws the cursor at its current coordinates on the double buffer
int draw_cursor(){

    //Calls the draw_xpm function with the cursor's image, at the cursor's position incremented by the drawing offset (-width/2 or -height/2) (so that the center of the sprite coincides with the cursor position), with the d_buf value for the buffer enum
    if( draw_xpm(cursor_obj->img, cursor_obj->x + CURSOR_DRAW_OFFSET, cursor_obj->y + CURSOR_DRAW_OFFSET, d_buf) != 0 ){
        return 1;
    }
    
    return 0;
}



/*
            MENU DRAWING FUNCTIONS
*/


//Function which draws the menu background on the background buffer
int draw_menu_background(){

    //Calls the draw_xpm function at the (0,0) position, with the b_buf value for the buffer enum
    if( draw_xpm(&menu_img[menu_back], 0, 0, b_buf) != 0 ){
        return 1;
    }

    return 0;
}

//Function which draws all menu elements on the double buffer
int draw_menu_elements(){

    //Copy the background to the double buffer
    copy_from_background();

    //Draw the current part of the title animation
    if( menu_obj -> title_v1 ){
        if( draw_xpm(&menu_img[title_1], TITLE_DRAW_X, TITLE1_DRAW_Y, d_buf) != 0 ){
            return 1;
        }
    }
    else{
        if( draw_xpm(&menu_img[title_2], TITLE_DRAW_X, TITLE2_DRAW_Y, d_buf) != 0 ){
            return 1;
        }
    }

    //draw the play button
    if( draw_xpm(menu_obj -> p_button, menu_obj -> play_button_x, menu_obj -> play_button_y, d_buf) != 0){
        return 1;
    }

    //draw the highscores button
    if( draw_xpm(menu_obj -> h_button, menu_obj -> highscore_x, menu_obj -> highscore_y, d_buf) != 0){
        return 1;
    }

    //Draw the cursor object on the double buffer
    if( draw_cursor() != 0 ){
        return 1;
    }
    return 0;
}



/*
            HIGHSCORE MENU DRAWING FUNCTIONS
*/


//Function which draws the highscore data on the background buffer
int draw_highscore_data(char **n, int *t, int *s){
    //n is a name without spaces
    //t comes in the format hhmmss

    for(int i = 0; i < MAX_HIGHSCORES; i++){

        uint16_t current_y = HIGHSCORE_START_Y + i * HIGHSCORE_Y_INCREMENT;

        //draw the highscore name
        if( draw_hs_name(n[i], HS_NAME_START_X, current_y) != 0 ){
            printf("Error drawing name %d.\n", i);
            return 1;
        }

        //draw the highscore time
        if( draw_hs_time(t[i], HS_TIME_UNIT_X, current_y) != 0 ){
            printf("Error drawing time %d.\n", i);
            return 1;
        }

        //draw the highscore score
        if( draw_hs_score(s[i], HS_SCORE_UNIT_X, current_y) != 0 ){
            printf("Error drawing time %d.\n", i);
            return 1;
        }
        
    }
    copy_from_background();
    update_vram();

    return 0;
}

//Function which draws a name at the specified coordinates
int draw_hs_name(char *to_draw, uint16_t x_pos, uint16_t y_pos){

    char current_char;
    int str_pos = 0;
    
    do{
        //get the current char from the string, in lower case
        current_char = (char) tolower( (int) to_draw[str_pos]);
        //get its corresponding image
        Image *current_image = &letters[current_char - 'a'];
        uint16_t current_x = x_pos + str_pos * current_image->image_info.width;

        //draw the xpm image
        if( draw_xpm(current_image, current_x, y_pos, b_buf) != 0 ){
            return 1;
        }

        //increment the position
        str_pos++;

    }while(current_char != '\0');

    return 0;
}

//Function which draws a time on the b buffer, at the specified coordinates
int draw_hs_time(int to_draw, uint16_t x_pos, uint16_t y_pos){

    //We need to write 6 digits, 2 at a time
    for( unsigned int i = 0; i < 6; i++){
        
        //gets the image, and updates the position we're drawing in
        Image *current_image = &numbers[to_draw % 10];
        uint16_t current_x = x_pos - i * current_image->image_info.width;
        
        //print current unit digit
        if( draw_xpm(current_image, current_x, y_pos, b_buf) != 0 ) {
            return 1;
        }

        //divide the number so we get the next set of digits
        to_draw /= 10;
    }

    //draw double dots
    if(draw_xpm(&numbers[10], HS_DOT_1, y_pos, b_buf) != 0 ){
        return 1;
    }
    if(draw_xpm(&numbers[10], HS_DOT_2, y_pos, b_buf) != 0 ){
        return 1;
    }

    return 0;
}

//Function which draws the score on 
int draw_hs_score(int to_draw, uint16_t x_pos, uint16_t y_pos){

    //We need to write 5 digits
    for( unsigned int i = 0; i < 5; i++){
        
        //gets the image, and updates the position we're drawing in
        Image *current_image = &numbers[to_draw % 10];
        uint16_t current_x = x_pos - i * current_image->image_info.width;
        
        //print current unit digit
        if( draw_xpm(current_image, current_x, y_pos, b_buf) != 0 ) {
            return 1;
        }

        //divide the number so we get the next set of digits
        to_draw /= 10;
    }

    return 0;
}

//Function which draws the menu background and the highscore data on the background buffer
int draw_highscore_background(char **n, int *t, int *s){

    //Calls the draw_xpm function at the (0,0) position, with the b_buf value for the buffer enum
    if( draw_xpm(&menu_img[high_back], 0, 0, b_buf) != 0 ){
        return 1;
    }

    if( draw_highscore_data(n, t, s) != 0 ){
        printf("Unable to draw the highscore data on the background buffer.\n");
        return 1;
    }

    return 0;
}



/*
            NEW HIGHSCORE SAVING SCREEN DRAWING FUNCTIONS
*/


//Function which draws the new highscore screen on the background buffer
int draw_new_highscore_screen(){

    //Calls the draw_xpm function at the desired position, with the b_buf value for the buffer enum
    if( draw_xpm(&menu_img[new_high], NEW_HIGHSCORE_X, NEW_HIGHSCORE_Y, b_buf) != 0 ){
        return 1;
    }

    return 0;
}



/*
            GAME DRAWING FUNCTIONS
*/


//Function which draws the game background on the background buffer
int draw_game_background(){

    //Calls the draw_xpm function at the (0,0) position, with the b_buf value for the buffer enum
    if( draw_xpm(&game_img[game_back], 0, 0, b_buf) != 0 ){
        return 1;
    }

    return 0;
}

//Function which draws the player at its current coordinates on the double buffer
int draw_player(){

    //Calls the draw_xpm function with the player's image,
    //at the player's position,
    //with the d_buf value for the buffer enum
    if( draw_xpm(player_obj->img, player_obj->x, player_obj->y, d_buf) != 0 ){
        return 1;
    }
    
    return 0;
}


//Function which draws the special enemy
int draw_special(){
    if(special->version1){
        if( draw_xpm(special->img_1, special->x, special->y, d_buf) != 0 ){
            return 1;
        }
    }
    else{
        if( draw_xpm(special->img_2, special->x, special->y, d_buf) != 0 ){
            return 1;
        }
    }
    return 0;
}


//Function which draws all enemies on the double buffer
int draw_all_enemies(){

    //Iterate through the enemy array
    for( unsigned int e = 0; e < ENEMY_NUMBER; e++){
        //Reference to the current enemy
        Enemy *current_enemy = enemies[e];

        //Check if the current enemy exists
        if(current_enemy == NULL){
            continue;
        }
        
        //Calls the draw_xpm function with the desired enemy image,
        //at the enemy's position
        //with the d_buf value for the buffer enum
        if(current_enemy->version1){
            //if we want to draw part 1 of the enemy animation
            Image* to_draw = current_enemy->img_1;
            if( draw_xpm(to_draw, current_enemy->x, current_enemy->y, d_buf) != 0 ){
                return 1;
            }
        }
        else{
            //if we want to draw part 2 of the enemy animation
            Image* to_draw = current_enemy->img_2;
            if( draw_xpm(to_draw, current_enemy->x, current_enemy->y, d_buf) != 0 ){
                return 1;
            }
        }
    }

    
    //If a special enemy exists
    if( special != NULL ){
        if( draw_special() != 0 ) {
            return 1;
        }
    }

    return 0;
}


//Function that draws all projectiles on the double buffer
int draw_projectiles(){
    
    //Iterate through the player_projectiles array
    for( unsigned int p = 0; p < MAX_PLAYER_PROJ; p++){

        //Reference to the current projectile
        Player_projectile *current_projectile = player_projectiles[p];

        //If the projectile at that position is not a null pointer, we can draw
        if( current_projectile != NULL ){

            //Iterate through the points of the projectile
            for( unsigned int i=0; i < MAX_PLAYER_PROJ; i++){
                
                //Get the point's x and y values
                int16_t x = current_projectile->line[2*i];
                int16_t y = current_projectile->line[2*i + 1];
                
                //Draw the pixel at the correct position with the projectile color,
                //and the d_buf value for the buffer enum
                if( draw_pixel(x, y, PROJECTILE_COLOR, d_buf) != 0 ){
                    printf("Error printing pixel at (%d, %d)\n", x, y);
                    return 1;
                }
            }
        }
    }

    //Iterate through the player_projectiles array
    for( unsigned int p = 0; p < MAX_ENEMY_PROJ; p++){
        Enemy_projectile *current_projectile = enemy_projectiles[p];
        if( current_projectile != NULL ){
            if( draw_xpm(current_projectile -> img, current_projectile -> x, current_projectile -> y, d_buf) != 0 ){
                printf("Error drawing an enemy projectile.\n");
                return 1;
            }
        }
    }

    return 0;
}


//Function which copies the background buffer contents to the double buffer,
//then draws all other objects on top
int draw_game_objects(){

    //Copy the background to the double buffer
    copy_from_background();

    //Draw the player object on the double buffer
    if( draw_player() != 0 ){
        return 1;
    }

    //Draw the enemy objects on the double buffer
    if( draw_all_enemies() != 0 ){
        return 1;
    }

    //Draw the projectile objects on the double buffer
    if( draw_projectiles() != 0 ){
        return 1;
    }

    //Draw the UI elements
    if ( draw_ui() != 0 ){
        return 1;
    }

    //Draw the cursor object on the double buffer
    if( draw_cursor() != 0 ){
        return 1;
    }
    
    return 0;
}



/*
            GAME UI DRAWING FUNCTIONS
*/


//Function which draws the player's hp value
int draw_hp(){

    int player_hp = player_obj -> hp;

    //the player can only have 99 hp max
    for( int i = 0; i < 2; i++){
        //print current unit digit
        if( draw_xpm(&numbers[player_hp % 10], HP_UNITS_X - 32*i, UI_ELEM_FIRST_Y, d_buf) != 0 ) {
            return 1;
        }
        //divide the number so we get the next set of digits
        player_hp /= 10;
    }
    return 0;
}


//Function which draws the player's current available ammunition
int draw_ammo(){
    int available = MAX_PLAYER_PROJ;

    for( unsigned int i = 0; i < MAX_PLAYER_PROJ; i++){
        if( player_projectiles[i] != NULL ){
            available--;
        }
    }

    //the player can only have up to 2 digits of ammo
    for( int i = 0; i < 2; i++){
        //print current unit digit
        if( draw_xpm(&numbers[available % 10], AMMO_UNITS_X - 32*i, UI_ELEM_FIRST_Y, d_buf) != 0 ) {
            return 1;
        }
        //divide the number so we get the next set of digits
        available /= 10;
    }

    return 0;
}


//Function which draws the player's current points
int draw_points(){
    
    int points = player_obj->points;

    //the player can have up to 99999 points (5 digits)
    for( unsigned int i = 0; i < 5; i++){
        //print current unit digit
        if( draw_xpm(&numbers[points % 10], POINTS_UNITS_X - 32*i, UI_ELEM_FIRST_Y, d_buf) != 0 ) {
            return 1;
        }
        //divide the number so we get the next set of digits
        points /= 10;
    }

    return 0;
}


//Function which draws all of the UI elements
int draw_ui(){

    //Draw the player hp
    if( draw_hp() != 0 ){
        return 1;
    }
    
    //Draw the available ammo
    if( draw_ammo() != 0 ){
        return 1;
    }
    
    //Draw the points
    if( draw_points() != 0 ){
        return 1;
    }

    return 0;
}


/*
            ANIMATION HANDLING FUNCTIONS
*/


//Function which swaps the menu animation
void change_title_animation(){
    menu_obj -> title_v1 = !(menu_obj -> title_v1);
}


//Function which changes the image of the enemy we're drawing next
void change_enemies_animation(){
    for(unsigned int e = 0; e < ENEMY_NUMBER; e++){
        if(enemies[e] == NULL){
            continue;
        }
        enemies[e]->version1 = !(enemies[e]->version1);
    }
}
