#ifndef _SCANCODE_DEFINES_H_
#define _SCANCODE_DEFINES_H_

/** @defgroup scancode_defines scancode_defines
 * @{
 * 
 * Scancodes and related constants for using when receiving keyboard interrupts
 */

//scancodes
#define A_MAKECODE      0x1E  /*!< the A key's makecode */
#define D_MAKECODE      0x20  /*!< the D key's makecode */
#define BACKSPACE_MAKE  0x0E  /*!< the backspace's makecode */
#define ENTER_MAKE      0x1C  /*!< the enter key's makecode */
#define ESC_BREAKCODE   0x81  /*!< the esc key's breakcode */

/*
      For use with the 1st scancode to char "map"
*/
#define MAKE_OFFSET_1   0x10  /*!< what we'll subtract from the scancode to get the char from our "map", if the key pressed is on the first row of letters (Q to P) */
#define MAKE_1_MIN      0x10  /*!< equivalent to Q */
#define MAKE_1_MAX      0x19  /*!< equivalent to P */

/*
      For use with the 2nd scancode to char "map"
*/
#define MAKE_OFFSET_2   0x1E  /*!< what we'll subtract from the scancode to get the char from our "map", if the key pressed is on the second row of letters (A to L) */
#define MAKE_2_MIN      0x1E  /*!< equivalent to A */
#define MAKE_2_MAX      0x26  /*!< equivalent to L */

/*
      For use with the 3rd scancode to char "map"
*/
#define MAKE_OFFSET_3   0x2C  /*!< what we'll subtract from the scancode to get the char from our "map", if the key pressed is on the third row of letters (Z to M) */
#define MAKE_3_MIN      0x2C  /*!< equivalent to Z */
#define MAKE_3_MAX      0x32  /*!< equivalent to M */

#endif
