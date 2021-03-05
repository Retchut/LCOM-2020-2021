#ifndef _GAME_H_
#define _GAME_H_

/** @defgroup game game
 * @{
 * 
 * Functions for handling the game
 */

/**
 * @brief Initializes the logic data struct, upon starting gameplay
 * 
 * @return 0 on success, non-zero otherwise
 */
void initialize_logic_data();

/**
 * @brief Prepares the game
 * 
 * @details Draws the game background on the background buffer and calls initialize_logic_data()
 * @return 0 on success, non-zero otherwise
 * @see initialize_logic_data()
 */
int prepare_game();

/**
 * @brief Function which readies the game for a new round
 * 
 * @return 0 on success, non-zero otherwise
 */
int new_round();

/**
 * @brief Ends the game
 * @details simply calls destroy_all_game_entities(), when called
 * @return 0 on success, non-zero otherwise
 */
void end_game();

/**
 * @brief Core gameplay loop
 * 
 * @return 0 on success, non-zero otherwise
 */
int game_loop(int tick);


/**
 * @brief Handles the data received from the keyboard interrupts, during the game
 * 
 * @return 0 on success, non-zero otherwise
 */
void game_kbd_handler(uint8_t scancode);

/**
 * @brief Handles the data received from the mouse interrupts, during the game
 * 
 * @return 0 on success, non-zero otherwise
 */
int game_mouse_handler(struct packet *read_packet);

#endif /* _GAME_H_ */
