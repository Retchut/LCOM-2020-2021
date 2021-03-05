#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

/** @defgroup keyboard keyboard
 * @{
 * 
 * Functions for handling the keyboard interrupts
 */

/**
 *@brief Subscribes keyboard interrupts
 *
 *@return Returs 0 upon success, non-zero otherwise
 */
int (kbd_subscribe_int)();

/**
 *@brief Unubscribes keyboard interrupts
 *
 *@return Returs 0 upon success, non-zero otherwise
 */
int (kbd_unsubscribe_int)();

/**
 *@brief Keyboard input handler
 *
 *@details Checks if we can read a valid scancode. If we can, it reads the scancode,
            and stores it on the global variable kbd_output_buffer
 */
void (kbc_ih)();

#endif /* _KEYBOARD_H_ */
