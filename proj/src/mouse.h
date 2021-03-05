#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

/** @defgroup mouse mouse
 * @{
 * 
 * Functions for handling the mouse interrupts
 */

#include <lcom/lcf.h>
#include "i8042.h"

/*!
 *@brief Subscribes mouse interrupts
 * 
 *@return Returs 0 upon success, non-zero otherwise
 */
int (ms_subscribe_int)();

/**
 *@brief Unubscribes mouse interrupts
 * 
 *@return Returs 0 upon success, non-zero otherwise
 */
int (ms_unsubscribe_int)();

/**
 *@brief Evaluates whether or not we can write a command to the kbc control register
 * 
 *@return Returs 0 upon success, non-zero otherwise
 */
int (can_write_to_ctrl_reg)();

/**
 *@brief Issues the argument command specified in its first argument
 * 
 *@details First writes the WRITE_BYTE_TO_MOUSE to the CTRL_REG, then the
            command parameter to the IN_BUF
 *@param port Port we're writing the command to
 *@param argument Command to be written to the kbc's control register
 *@return Returs 0 upon success, non-zero otherwise
 */
int (ms_issue_arg_command)(uint8_t argument);

/**
 *@brief Enables mouse data reporting and subscribes mouse interrupts
 * 
 *@return Returs 0 upon success, non-zero otherwise
 */
int ms_init();

int mouse_read_discard();

/**
 *@brief Disables mouse data reporting, and unsubscribes mouse interrupts
 * 
 *@return Returs 0 upon success, non-zero otherwise
 */
int ms_reset();

/**
 *@brief Mouse input handler
 * 
 *@details Checks if we can read a valid scancode. If we can, it reads the scancode,
            and stores it on the global variable ms_output_buffer. If we read an
 */
void (mouse_ih)();

/**
 *@brief Function that parses a mouse packet
 * 
 *@param toParse Mouse packet to be parsed
 */
void (parse_packet)(struct packet* toParse);

#endif /* _LCOM_MOUSE_H_ */
