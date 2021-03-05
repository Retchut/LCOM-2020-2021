#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "video.h"  //necessary for access to the buffer enum


/**
 * @brief Struct that holds an image's information and color map
 */
typedef struct Image{
    xpm_image_t image_info; /*!< the image's info */
    uint8_t *color_map;     /*!< the image's color map */
} Image;


/**
 * @brief Enumerated type for specifying which enemy animation we're accessing
 */
enum enemy_images {
    type1_A = 0,    /*!< type 1 animation A */
    type1_B = 1,    /*!< type 1 animation B */
    type2_A = 2,    /*!< type 2 animation A */
    type2_B = 3,    /*!< type 2 animation B */
    special_A = 4,  /*!< special enemy animation A */
    special_B = 5,  /*!< special enemy animation B */
    enemy_proj = 6  /*!< enemy projectile sprite */
};


/**
 * @brief Enumerated type for holding the menu's images
 */
enum menu_images{
  menu_back = 0,    /*!< the menu's background image */
  title_1 = 1,      /*!< the first title image */
  title_2 = 2,      /*!< the second title image */
  p_butt = 3,       /*!< the play button's image */
  h_butt = 4,       /*!< the highscore button's image */
  high_back = 5,    /*!< the highscore's background image */
  new_high = 6      /*!< the new highscore screen's background image */
};


/**
 * @brief Enumerated type for holding the game's non-enemy images
 */
enum game_images{
  game_back = 0,    /*!< the game's background image */
  player_img = 1,   /*!< the player's image */
  crosshair_img = 2 /*!< the crosshair's image */
};


/**
 * @brief Enumerated type for holding the game's non-enemy images
 */
enum letter_images{
  letter_a = 0,  /*!< a letter image */
  letter_b = 1,  /*!< b letter image */
  letter_c = 2,  /*!< c letter image */
  letter_d = 3,  /*!< d letter image */
  letter_e = 4,  /*!< e letter image */
  letter_f = 5,  /*!< f letter image */
  letter_g = 6,  /*!< g letter image */
  letter_h = 7,  /*!< h letter image */
  letter_i = 8,  /*!< i letter image */
  letter_j = 9,  /*!< j letter image */
  letter_k = 10, /*!< k letter image */
  letter_l = 11, /*!< l letter image */
  letter_m = 12, /*!< m letter image */
  letter_n = 13, /*!< n letter image */
  letter_o = 14, /*!< o letter image */
  letter_p = 15, /*!< p letter image */
  letter_q = 16, /*!< q letter image */
  letter_r = 17, /*!< r letter image */
  letter_s = 18, /*!< s letter image */
  letter_t = 19, /*!< t letter image */
  letter_u = 20, /*!< u letter image */
  letter_v = 21, /*!< v letter image */
  letter_w = 22, /*!< w letter image */
  letter_x = 23, /*!< x letter image */
  letter_y = 24, /*!< y letter image */
  letter_z = 25  /*!< z letter image */
};



/*
            GENERIC IMAGE LOADING AND DRAWING FUNCTIONS
*/


/**
 * @brief Loads images from the xpm files, to be used by the game
 * 
 * @details Also initializes Image objects for every image loaded
 */
void load_images();


/**
 * @brief Draws an xpm according to some parameters
 * 
 * @param image     Pointer to the image object we want to draw
 * @param x_pos     X position we start to draw the xpm in
 * @param y_pos     Y position we start to draw the xpm in
 * @param to_draw_in  Buffer we're drawing in
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_xpm(struct Image *image, uint16_t x_pos, uint16_t y_pos, enum buffer to_draw_in);



/*
            CURSOR DRAWING FUNCTION
*/


/**
 * @brief Draws the cursor on the buffer used for double buffering
 * 
 * @details Calls draw_xpm with the d_buf enum value, at the cursor's position
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_cursor();



/*
            MENU DRAWING FUNCTIONS
*/


/**
 * @brief Draws the menu background on the background buffer
 * 
 * @details Calls draw_xpm with the b_buf enum value
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_menu_background();


/**
 * @brief Draws the menu elements on the double buffer
 * 
 * @details Calls draw_xpm with the d_buf enum value
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_menu_elements();



/*
            HIGHSCORE MENU DRAWING FUNCTIONS
*/


/**
 * @brief Draws the highscore background on the background buffer
 * 
 * @details Calls draw_xpm with the b_buf enum value and draw_highscore_data
 * @param n Pointer to the array of names
 * @param t Pointer to the array of times
 * @param s Pointer to the array of scores
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_highscore_background(char **n, int *t, int *s);


/**
 * @brief Draws the highscore data on the background buffer
 * 
 * @details Calls draw_xpm with the b_buf enum value
 * @param n Pointer to the array of names
 * @param t Pointer to the array of times
 * @param s Pointer to the array of scores
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_highscore_data(char **n, int *t, int *s);


/**
 * @brief Draws a highscore name on the background buffer
 * 
 * @details Calls draw_xpm with the b_buf enum value
 * @param to_draw Pointer to the string we're writing
 * @param x_pos   X position we start to draw the xpm in
 * @param y_pos   Y position we start to draw the xpm in
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_hs_name(char *to_draw, uint16_t x_pos, uint16_t y_pos);


/**
 * @brief Draws the highscore time on the background buffer
 * 
 * @details Time is in format hhmmss. We want to print it in the format hh:mm:ss. Calls draw_xpm with the b_buf enum value
 * @param to_draw Time we're writing
 * @param x_pos   X position we start to draw the xpm in
 * @param y_pos   Y position we start to draw the xpm in
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_hs_time(int to_draw, uint16_t x_pos, uint16_t y_pos);


/**
 * @brief Draws the highscore score on the background buffer
 * 
 * @details Calls draw_xpm with the b_buf enum value
 * @param to_draw Score we're writing
 * @param x_pos   X position we start to draw the xpm in
 * @param y_pos   Y position we start to draw the xpm in
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_hs_score(int to_draw, uint16_t x_pos, uint16_t y_pos);



/*
            NEW HIGHSCORE SAVING SCREEN DRAWING FUNCTIONS
*/


/**
 * @brief Draws the new highscore screen on the background buffer
 * 
 * @details Calls draw_xpm with the b_buf enum value
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_new_highscore_screen();



/*
            GAME DRAWING FUNCTIONS
*/


/**
 * @brief Draws the game background on the background buffer
 * 
 * @details Calls draw_xpm with the b_buf enum value
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_game_background();


/**
 * @brief Draws the player on the buffer used for double buffering
 * 
 * @details Calls draw_xpm with the d_buf enum value, at the player's position
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_player();


/**
 * @brief Draws the special enemy on the double buffer
 * 
 * @details Calls draw_xpm with the d_buf enum value, at the special enemy's position
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_special();


/**
 * @brief Draws each enemy on the buffer used for double buffering
 * 
 * @details Calls draw_xpm with the d_buf enum value, at each enemy's position
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_all_enemies();


/**
 * @brief Draws all projectiles on the screen on the buffer used for double buffering
 * 
 * @details Calls draw_xpm with the d_buf enum value, at each projectile's position
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_projectiles();


/**
 * @brief Function which copies the background buffer contents to the double buffer, then draws all other objects on top
 * 
 * @details Calls copy_background(), draw_player(), draw_all_enemies(), draw_projectiles() and draw_cursor, in this order
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_game_objects();



/*
            GAME UI DRAWING FUNCTIONS
*/


/**
 * @brief Draws the number of hp the player has
 * 
 * @details Calls draw_xpm with the d_buf enum value, and the correct number xpm, at the correct location
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_hp();

/**
 * @brief Draws the number of ammunition the player has
 * 
 * @details Calls draw_xpm with the d_buf enum value, and the correct number xpm, at the correct location
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_ammo();

/**
 * @brief Draws the number of points the player has
 * 
 * @details Calls draw_xpm with the d_buf enum value, and the correct number xpm, at the correct location
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_points();

/**
 * @brief Draws all ui elements of points the player has
 * 
 * @details Calls draw_hp, draw_ammo and draw_hp
 * @return 0 on success, non-zero otherwise
 * @see video.h
 */
int draw_ui();



/*
            ANIMATION HANDLING FUNCTIONS
*/


/**
 * @brief Changes the menu title image we're drawing next for all enemies
 */
void change_title_animation();


/**
 * @brief Changes the enemy image we're drawing next for all enemies
 */
void change_enemies_animation();


#endif /* _SPRITE_H_ */
