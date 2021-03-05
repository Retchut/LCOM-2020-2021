#include <lcom/lcf.h>
#include <lcom/lab5.h>

#include "video_gr.h"

static char *video_mem; /* frame-buffer VM address */
static char *buffer;    /* video buffer for implementing double buffering */

static unsigned int vram_base;     //VRAM's physical address
static unsigned int vram_size;     //VRAM's size, but we can use the frame buffer's size instead

//horizontal and vertical resolutions
static unsigned int h_res;	        /* Horizontal resolution in pixels (pixels on each line) */
static unsigned int v_res;	        /* Vertical resolution in pixels (pixels on each column) */
static unsigned int bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned int bytes_per_pixel;/* Number of bytes per pixel */

//color masks
static uint8_t red_mask_size;   //number of pixels on the red mask
static uint8_t green_mask_size; //number of pixels on the green mask
static uint8_t blue_mask_size;  //number of pixels on the blue mask

static bool indexed_mode = false;

void* (vg_init)(uint16_t mode){
    
    //initialize struct used to save the info received from the 0x01 function
    vbe_mode_info_t info;
    
    //get mode info (using the function previously developed by us)
    if( get_mode_info(mode, &info) != 0 ){
        printf("Could not get mode info.\n");
        return NULL;
    }

    //if the mode we're initiating is mode 0x105, it's an indexed color mode
    if(mode == 0x105){
        indexed_mode = true;
    }

    //initiallize global variables
    h_res = info.XResolution;
    v_res = info.YResolution;
    bits_per_pixel = info.BitsPerPixel;
    bytes_per_pixel = (bits_per_pixel + 7)/8;
    red_mask_size = info.RedMaskSize;
    green_mask_size = info.GreenMaskSize;
    blue_mask_size = info.BlueMaskSize;

    vram_base = info.PhysBasePtr;

    vram_size = h_res * v_res * bytes_per_pixel;

    //declare variables to change process's perms, and map the memory
    struct minix_mem_range mr;
    int r;

    /* Allow memory mapping */
    mr.mr_base = (phys_bytes) vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if( OK != ( r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr )))
        panic("sys_privct1(ADD_MEM) failed: %d\n", r);

    /* Map memory */
    video_mem = vm_map_phys(SELF, (void*) mr.mr_base, vram_size);

    if(video_mem == MAP_FAILED){
        panic("Couldn't map video memory.\n");
    }

    /* Allocate Buffer Memory */
    buffer = (char *) malloc(vram_size);

    /* Clean alocated memory and buffer */
    memset(video_mem, 0, vram_size);
    memset(buffer, 0, vram_size);
  
    //set graphics mode to the desired mode
    set_graphics_mode(mode);

    return video_mem;
}

int (draw_pixel)(uint16_t x, uint16_t y, uint32_t color){    
    //if our pixel is outside our resolution's bounds, and we can't draw it
    if( x < 0 || y < 0 || x > h_res || y > v_res){
        printf("Pixel not inside resolution bounds.\n");
        return 1;
    }
    else{

        //get the number of pixels we have to add to the base address, to draw the pixel we want to draw
        int pixel_offset = (y * h_res) + x;
        //initialize a temporary buffer, so we don't change the address stored in our double buffer
        char* temp_buffer = buffer + (pixel_offset * bytes_per_pixel);
        //printf("color : %x", color);
        //initialize a temporary color variable, so we don't change the color we want to draw
        //uint32_t temp_color = color;

        //loop to write byte by byte into the buffer
        for(unsigned i=0; i<bytes_per_pixel; i++){
            //write one byte into the buffer
            (*temp_buffer) = color;
            //get the next byte
            color = color >> 8;
            //increment the buffer
            temp_buffer++;
        }

        return 0;
    }
    
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
    //pointer initially holding the address of the first pixel to paint.
    for(uint16_t i = 0; i < len; i++){
        if( draw_pixel(x + i, y, color) != 0 ){
            printf("Error drawing pixel (x = %d, y = %d).\n", x, y);
            return 1;
        }
    }
    return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
    for(uint16_t i = 0; i < height; i++){
        if( vg_draw_hline(x, y + i, width, color) != 0 ){
            printf("Error drawing line (y = %d).\n", y);
            return 1;
        }
    }
    memcpy(video_mem, buffer, vram_size);
    return 0;
}

uint8_t generate_mask(uint8_t mask_size, uint8_t offset){
    uint8_t mask = 0;
    for(int i = 0; i < mask_size; i++){
        mask = mask << 1;
        mask |= 1;
    }
    mask = mask << offset;
    return mask;
}

int (draw_filled_grid)(uint8_t no_rectangles, uint32_t first, uint8_t step){
    //get the length of the rectangles (they get truncated downwards - 800/3 = 266)
    uint16_t rectangle_width = h_res / no_rectangles;
    uint16_t rectangle_height = v_res / no_rectangles;

    //calculate the extra pixels we have to fill in with black
    //uint16_t extra_x_pixels = h_res - (rectangle_width * no_rectangles);
    //uint16_t extra_y_pixels = v_res - (rectangle_height * no_rectangles);

    uint32_t current_color = first;

    //draw line by line
    for(size_t line = 0; line < no_rectangles; line++){
        //draw a square on each column of the line
        for(size_t col = 0; col < no_rectangles; col++){
            //get the next components
            if(indexed_mode){
                current_color = (first +( line * no_rectangles + col) * step ) % (1 << bits_per_pixel);
                current_color = current_color % 256;
            }
            else{
                //get first R, G and B values
                uint8_t first_R = (first >> (green_mask_size + blue_mask_size)) & generate_mask(red_mask_size, 0);
                uint8_t first_G = (first >> (blue_mask_size)) & generate_mask(green_mask_size, 0);
                uint8_t first_B = first & generate_mask(blue_mask_size, 0);

                uint8_t current_R = (first_R + col * step) % (1 << red_mask_size);
                uint8_t current_G = (first_G + line * step) % (1 << green_mask_size);
                uint8_t current_B = (first_B + (col + line) * step) % (1 << blue_mask_size);

                //shift R to the left by the green and blue mask sizes
                current_color = (current_R << (green_mask_size + blue_mask_size)) | (current_G << blue_mask_size) | current_B;
            }
            
            //printf("Square (%d, %d) - color = %X\n", line, col, current_color);

            if(vg_draw_rectangle(col * rectangle_width, line * rectangle_height, rectangle_width, rectangle_height, current_color) != 0){
                printf("Error drawing rectangle (%d, %d).\n", line, col);
                return 1;
            }
        }   
    }
    
    return 0;
}

int (draw_xpm)(xpm_image_t img_info, uint8_t *img_color_map, uint16_t x_pos, uint16_t y_pos){

    //WE'RE USING AN INDEXED COLOR MODE WITH 8 BITS (1 BYTE) PER PIXEL

    //number of pixels per line on the img (number of total columns)
    uint16_t x_pixels = img_info.width;
    //number of pixels per column on the img (number of total lines)
    uint16_t y_pixels = img_info.height;

    //iterate through the lines
    for(unsigned int line = 0; line < y_pixels; line++){

        //iterate through each column on one line
        for(unsigned int col = 0; col < x_pixels; col++){

            //read the current color
            uint8_t current_color = *(img_color_map + (line*x_pixels) + col);

            //draw the pixel at that location
            if( draw_pixel(x_pos + col, y_pos + line, current_color) != 0 ){
                printf("Error printing the xpm image.\n");
                return 1;
            }
        }
    }

    //copy memory from the buffer into the vram
    memcpy(video_mem, buffer, vram_size);

    return 0;
}

int (erase_xpm)(xpm_image_t img_info, uint16_t x_pos, uint16_t y_pos){
    //WE'RE USING AN INDEXED COLOR MODE WITH 8 BITS (1 BYTE) PER PIXEL

    //number of pixels per line on the img (number of total columns)
    uint16_t x_pixels = img_info.width;
    //number of pixels per column on the img (number of total lines)
    uint16_t y_pixels = img_info.height;

    //uint32_t transparency = xpm_transparency_color(img_info.type);

    //iterate through the lines
    for(unsigned int line = 0; line < y_pixels; line++){

        //iterate through each column on one line
        for(unsigned int col = 0; col < x_pixels; col++){

            //draw the pixel at that location
            if( draw_pixel(x_pos + col, y_pos + line, 0xFF0000) != 0 ){
                printf("Error printing the xpm image.\n");
                return 1;
            }
        }
    }

    //copy memory from the buffer into the vram
    memcpy(video_mem, buffer, vram_size);

    return 0;
}
