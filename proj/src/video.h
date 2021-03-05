#ifndef _VIDEO_H_
#define _VIDEO_H_

/**
 * @brief Enumerated type for specifying the buffer we're writing in
 */
enum buffer {
  b_buf = 0,  /*!< background_buffer */
  d_buf = 1,  /*!< double_buffer */
  vram = 2    /*!< video_mem */
  };

/*!
 * @brief Initializes the video module in mode 0x115 using a linear frame buffer, and maps VRAM to the process's address space.
 * 
 * @details Initializes static global variables with the resolution of the screen, and the color depth (i.e the no. of bits per pixel).
 * @return 0 on success, non-zero otherwise
 */
int graphics_init();

/*!
 * @brief //Function which draws one pixel on the screen with the desired color, in the desired buffer
 * 
 * @details x coordinates start at 0 and increase from left to right
 *          and y coordinates start at 0 and increase from top to bottom
 * @param x horizontal coordinate of the pixel to draw
 * @param y vertical coordinate of the pixel to draw
 * @param color color to set the pixel to
 * @param to_draw_in  Buffer we're drawing in
 * @return 0 on success, non-zero otherwise
 */
int draw_pixel(uint16_t x, uint16_t y, uint32_t color, enum buffer to_draw_in);

/*!
 * @brief Copies the background buffer's contents into the double buffer
 */
void copy_from_background();

/*!
 * @brief Copies the double buffer's contents into the background buffer
 */
void copy_to_background();

/*!
 * @brief Updates the video ram
 * 
 * @details Copies the content stored in the double buffer into the video ram
 */
void update_vram();

#endif /* _VIDEO_H_ */
