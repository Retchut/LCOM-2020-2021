#ifndef _PROJ_TIMER_H_
#define _PROJ_TIMER_H_

/** @defgroup proj_timer proj_timer
 * @{
 * 
 * Functions for handling timer interrupts
 */

/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int(proj_timer_subscribe_int)();

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(timer_unsubscribe_int)();

/**
 * @brief Timer 0 interrupt handler
 *
 * Increments counter
 */
void(timer_int_handler)();

#endif /* _PROJ_TIMER_H_ */
