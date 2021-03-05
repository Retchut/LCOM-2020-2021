#ifndef _MENU_H_
#define _MENU_H_

/** @defgroup menu menu
 * @{
 * 
 * Functions for managing the menu
 */

struct Image;

typedef struct MenuItems{
  bool title_v1;                      /*!< bool which tells us which version of the title to draw */
  struct Image *p_button;             /*!< play button image */
  int play_button_width;              /*!< play button's width */ 
  int play_button_height;             /*!< play button's height */
  int play_button_x;                  /*!< play button's x position */
  int play_button_y;                  /*!< play button's y position */
  struct Image *h_button;             /*!< highscore button's image */ 
  int highscore_width;                /*!< highscore button's width */ 
  int highscore_height;               /*!< highscore button's height */ 
  int highscore_x;                    /*!< highscore button's x position */ 
  int highscore_y;                    /*!< highscore button's y position */
} MenuItems;

/**
 * @brief Prepares the menu
 * 
 * @details Draws the menu background on the background buffer and initializes the MenuItems struct
 * @return 0 on success, non-zero otherwise
 */
int prepare_menu();

/**
 * @brief Closes the menu
 * 
 * @details Frees the MenuItems struct memory
 */
void close_menu();

/**
 * @brief Menu loop
 * 
 * @param tick The current timer counter
 * @return 0 on success, non-zero otherwise
 */
int menu_loop(int tick);

/**
 * @brief Handles the data received from the keyboard interrupts, during the game
 * 
 * @param scancode The scancode we have read
 * @return 0 on success, non-zero otherwise
 */
int menu_kbd_handler(uint8_t scancode);

/**
 * @brief Handles the data received from the mouse interrupts, during the game
 * 
 * @param read_packet The packet we have read
 * @return 0 on success, non-zero otherwise
 */
int menu_mouse_handler(struct packet *read_packet);

#endif /* _MENU_H_ */
