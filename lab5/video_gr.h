#ifndef _LCOM_VIDEO_GR_H_
#define _LCOM_VIDEO_GR_H_

#include "vbe.h"

/*!
 * @brief Initializes the video module in graphics mode using a linear frame buffer, and maps VRAM to the process's address space.
 * @details Initializes static global variables with the resolution of the screen, and the color depth (i.e the no. of bits per pixel).
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure
 */
void* (vg_init)(uint16_t mode);

/*!
 * @brief Draws a pixel by changing the video RAM.
 * @details x coordinates start at 0 and increase from left to right
 *          and y coordinates start at 0 and increase from top to bottom
 * @param x horizontal coordinate of the pixel to draw
 * @param y vertical coordinate of the pixel to draw
 * @param color color to set the pixel to
 * @return 0 on success, non-zero otherwise
 */
int (draw_pixel)(uint16_t x, uint16_t y, uint32_t color);

/*!
 * @brief Draws a horizontal line by changing the video RAM.
 * @details x coordinates start at 0 and increase from left to right
 *          and y coordinates start at 0 and increase from top to bottom
 * @param x horizontal coordinate of the initial point
 * @param y vertical coordinate of the initial point
 * @param len line's length in pixels
 * @param color color to set the pixels
 * @return 0 on success, non-zero otherwise
 */
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

/*!
 * @brief Draws a rectangle by changing the video RAM.
 * @details x coordinates start at 0 and increase from left to right
 *          and y coordinates start at 0 and increase from top to bottom
 * @param x horizontal coordinate of the initial point
 * @param y vertical coordinate of the initial point
 * @param width rectangle length in pixels
 * @param height rectangle height in pixels
 * @param color color to set the pixels
 * @return 0 on success, non-zero otherwise
 */
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

 /*!
  * @brief Generates a bitmask based on the input
  * @param mask_size size of the mask
  * @param mask_offset number of zeros to the right of the mask
  * @return generated mask
  */
 uint8_t generate_mask(uint8_t mask_size, uint8_t offset);

/*!
 * @brief Draws a grid of rectangles of no_rectangles x no_rectangles
 * @details All rectangles must have the same size. if the h_res/v_res are not multiples of no_rectangles, there
 *          will be a black stripe on the right/bottom of the screen, of minimum width.
 *          The color of each rectangle depends on the coordinates in the rectangle matrix (row, col).
 *          If the color mode is indexed: color_index(row, col) = (first +(row * no_rectangles + col) * step ) % (1 << BitsPerPixel)
 *          If the color mode is direct, the RGB components of the color are given by the expressions:
 *              -R(row, col) = (R(first) + col * step) % (1 << RedMaskSize)
 *              -G(row, col) = (G(first) + row * step) % (1 << GreenMaskSize)
 *              -B(row, col) = (B(first) + (col + row) * step) % (1 << BlueMaskSize)
 * @param no_rectangles number of rectangles in both the horizontal and vertical directions
 * @param first         color to be used in the first rectangle (top left corner of the screen)
 * @param step          difference between the r/g component in adjacent rectangles of the same row/column
 * @return 0 on success, non-zero otherwise
 */
int (draw_filled_grid)(uint8_t no_rectangles, uint32_t first, uint8_t step);

/*!
 *@brief Draws the xpm specified in the first argument, in the position given by (x_pos, y_pos)
 *@param img_info       Enum that holds data about the xpm (check comments)
 *@param img_color_map  Address of the array of the image colors
 *@param x_pos          X position to start drawing at
 *@param y_pos          Y position to start drawing at
 *@return 0 on success, non-zero otherwise
 */
int (draw_xpm)(xpm_image_t img_info, uint8_t *img_color_map, uint16_t x_pos, uint16_t y_pos);

/*!
 *@brief Erases the xpm specified in the first argument, in the position given by (x_pos, y_pos)
 *@param img_info       Enum that holds data about the xpm (check comments)
 *@param img_color_map  Address of the array of the image colors
 *@param x_pos          X position to start erasing at
 *@param y_pos          Y position to start erasing at
 *@return 0 on success, non-zero otherwise
 */
int (erase_xpm)(xpm_image_t img_info, uint16_t x_pos, uint16_t y_pos);

/**
 * @brief Struct that stores the information about an image.
 * 
 * type is the output format
 * width is the number of horizontal pixels
 * height is the number of vertical pixels
 * size is the number of bytes [width * height * bytes_per_pixel]
 * data is the pointer to the start of the image data
 */
// typedef struct {
//   enum xpm_image_type type;
//   uint16_t width;
//   uint16_t height;
//   size_t size;
//   uint8_t *data;
// } xpm_image_t;

/**
 * Reads a xpm-like pixmap defined in "map". Returns the address of the
 * allocated memory to where the image was read in the specified format (type).
 * Updates "img" with the image information. Returns NULL on error.
 * 
 * NOTE: If the required format does not support alpha channel (transparency/opacity),
 *       a corresponding chroma key green will be used instead (except for indexed mode).
 * 
 * Usage example, assuming that "my_xpm" stores the content of the XPM file:
 * <pre>
 *   // can be XPM_INDEXED, XPM_1_5_5_5, XPM_5_6_5, XPM_8_8_8 or XPM_8_8_8_8
 *   enum xpm_image_type type = XPM_5_6_5;
 *   xpm_image_t img;
 *   uint8_t *sprite = xpm_load(my_xpm, type, &img);
 * </pre>
 */
// uint8_t *xpm_load(xpm_map_t map, enum xpm_image_type type, xpm_image_t *img);

/**
 * Frees the dynamically allocated data pointed by data field.
 * Returns false on error.
 */
// bool xpm_free_data(xpm_image_t *img);

/**
 * Returns the color used as transparency for a given XPM image type or 0 if the type
 * is not valid.
 */
// uint32_t xpm_transparency_color(enum xpm_image_type type);

#endif /*   _LCOM_VIDEO_GR_H_     */
