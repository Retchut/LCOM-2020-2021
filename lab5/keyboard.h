#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

/*!
 *@brief Subscribes keyboard interrupts
 *@return Returs 0 upon success, non-zero otherwise
 */
int (kbd_subscribe_int)();

/*!
 *@brief Unubscribes keyboard interrupts
 *@return Returs 0 upon success, non-zero otherwise
 */
int (kbd_unsubscribe_int)();

/*!
 *@brief Keyboard input handler
 *@details Checks if we can read a valid scancode. If we can, it reads the scancode,
            and stores it on the global variable kbd_output_buffer
 */
void (kbc_ih)();

/*!
 *@brief Evaluates whether or not we can read a valid scancode
 *@return Returs 0 upon success, non-zero otherwise
 */
int can_read_scancode();

/*!
 *@brief Issues the command specified in its first argument
 *@param port Port to write the command in
 *@param command Command to be written to the kbc's control register
 *@return Returs 0 upon success, non-zero otherwise
 */
int kbd_issue_command(int port, uint8_t command);

/*!
 *@brief Reads the return value returned by a kbc command previously written to the kbc control register
 *@param returned_value Variable where we'll be storing the return value
 *@return Returs 0 upon success, non-zero otherwise
 */
int read_return_value(uint8_t *returned_value);

#endif /* _KEYBOARD_H_ */
