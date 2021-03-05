#ifndef _HIGHSCORE_H_
#define _HIGHSCORE_H_

/** @defgroup highscore highscore
 * @{
 * 
 * Functions for handling the highscores
 */

struct Image;

/**
 * @brief Loads the highscores from the highscores.txt file
 * 
 * @param n Pointer to the array of names
 * @param t Pointer to the array of times
 * @param s Pointer to the array of scores
 * @return 0 on success, non-zero otherwise
 */
int load_highscores(char **n, int *t, int *s);

/**
 * @brief Sorts the highscores using a simple bubble sort algorithm
 * 
 * @param n Pointer to the array of names
 * @param t Pointer to the array of times
 * @param s Pointer to the array of scores
 * @return 0 on success, non-zero otherwise
 */
int sort_highscores(char **n, int *t, int *s);

/**
 * @brief Saves the highscores to the highscores.txt file
 * 
 * @param n Pointer to the array of names to load from
 * @param t Pointer to the array of times to load from
 * @param s Pointer to the array of scores to load from
 * @return 0 on success, non-zero otherwise
 */
int save_highscores(char **n, int *t, int *s);

/**
 * @brief Prepares the menu
 * 
 * @details Draws the menu background on the background buffer
 * @return 0 on success, non-zero otherwise
 */
int prepare_highscore_menu();

/**
 * @brief Menu loop
 * 
 * @return 0 on success, non-zero otherwise
 */
int highscore_menu_loop();

/**
 * @brief Handles the data received from the keyboard interrupts, during the game
 * 
 * @param scancode The scancode we have read
 */
void highscore_menu_kbd_handler(uint8_t scancode);

/**
 * @brief Handles the data received from the mouse interrupts, while the highscore menu is open
 * 
 * @param read_packet The packet we have read
 */
void highscore_menu_mouse_handler(struct packet *read_packet);

/**
 * @brief Prepares the new_highscore screen
 * 
 * @details Draws the current double buffer's contents into the background buffer, then the new highscore image into the background buffer
 * @return 0 on success, non-zero otherwise
 */
int prepare_new_highscore();

/**
 * @brief Add a new highscore into our highscores arrays
 * 
 * @details After receiving a name from the player and retrieving the current data, replace the last items on our names, times and scores arrays with the parameters, and resort the highscores
 * @param n Pointer to the array of names
 * @param t Pointer to the array of times
 * @param s Pointer to the array of scores
 * @param new_name Pointer to the array of chars of the new name we want to save
 * @param new_time New time we want to save
 * @param new_score New score we want to save
 * @return 0 on success, non-zero otherwise
 */
int add_new_highscore(char **n, int *t, int *s, char *new_name, int new_time, int new_score);

/**
 * @brief Draws the name the player is editing on the double buffer
 * 
 * @param to_draw Name we're drawing
 * @return 0 on success, non-zero otherwise
 */
int draw_new_name(char *to_draw);

/**
 * @brief Draws the non-static new highscore menu images
 * 
 * @details Copies data from the background buffer into the double buffer, draws the name passed as parameter, and draws the cursor. Updates the vram.
 * @param name Name we're passing to draw_new_name()
 * @return 0 on success, non-zero otherwise
 * @see draw_new_name()
 */
int new_highscore_loop(char* name);

/**
 * @brief Handles the data received from the mouse interrupts, while the new highscore screen is open
 * 
 * @param read_packet The packet we have read
 * @param store_in The string where we'll be storing the null character
 * @param str_pos The position on the string where we're storing the null character
 */
void new_highscore_mouse_handler(struct packet *read_packet, char* store_in, int *str_pos);

/**
 * @brief Handles the data received from the keyboard interrupts, while the new highscore screen is open
 * 
 * @param scancode The scancode we read
 * @param store_in The string where we'll be storing the inputted character
 * @param str_pos The position on the string where we're storing the character
 */
void new_highscore_kbd_handler(uint8_t scancode, char* store_in, int *str_pos);

#endif /* _HIGHSCORE_H_ */
