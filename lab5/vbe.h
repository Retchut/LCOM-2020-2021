#ifndef _LCOM_VBE_H_
#define _LCOM_VBE_H_

#include <lcom/lcf.h>
#include <lcom/lab5.h>

#include "vbe_standard.h"

/*!
 * @brief Initializes the video module in graphics mode using a linear frame buffer.
 * @param mode 16-bit VBE mode to set
 * @return 0 on success, non-zero otherwise.
 */
int (set_graphics_mode)(uint16_t mode);

/*!
 * @brief Gets the information of a mode, and stores said information in the vbe_mode_info_t struct
 * @param mode The mode whose information we're obtaining.
 * @param info The struct wherein we're storing information about the mode.
 * @return 0 on success, non-zero otherwise
 */
int (get_mode_info)(uint16_t mode, vbe_mode_info_t *info);


#endif  /*    _LCOM_VBE_H_    */
