#include "lcom/lcf.h"

#include "vbe_funcs.h"
#include "video.h"
#include "video_defines.h"

//USE DEFINES FOR WHAT I ALREADY KNOW????

static char *video_mem;         /* frame-buffer VM address */
static char *double_buffer;     /* video buffer for implementing double buffering */
static char *background_buffer; /* buffer for holding the background data */

static unsigned int vram_base;     //VRAM's physical address
static unsigned int vram_size;     //VRAM's size

unsigned int bytes_per_pixel;       /* Number of bytes per pixel */

//Initializes the video module in mode 0x115 using a linear frame buffer, and maps VRAM to the process's address space.
int graphics_init(){
    
    //initialize struct used to save the info received from the 0x01 function
    vbe_mode_info_t info;
    
    //get mode info (using the function previously developed by us)
    if( get_mode_info(0x115, &info) != 0 ){
        printf("Could not get mode info.\n");
        return 1;
    }

    //initiallize global variables
    unsigned int bits_per_pixel = info.BitsPerPixel;
    bytes_per_pixel = (bits_per_pixel + 7)/8;

    vram_base = info.PhysBasePtr;
    vram_size = SCREEN_RES_X * SCREEN_RES_Y * bytes_per_pixel;

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
    double_buffer = (char *) malloc(vram_size);
    background_buffer = (char *) malloc(vram_size);

    /* Clean alocated memory and buffer */
    memset(video_mem, 0, vram_size);
    memset(double_buffer, 0, vram_size);
    memset(background_buffer, 0, vram_size);
  
    //set graphics mode to the desired mode
    if( set_graphics_mode(0x115) != 0 ){
        return 1;
    }

    return 0;
}

//Function which draws one pixel on the screen with the desired color, in the desired buffer
int draw_pixel(uint16_t x, uint16_t y, uint32_t color, enum buffer to_draw_in){

    //If the color is the transparency color, we don't draw
    if( color == CHROMA_KEY_GREEN_888 ){
        return 0;
    }

    //if our pixel is outside our resolution's bounds, and we can't draw it
    if( x < 0 || y < 0 || x > MAX_X_PIXEL || y > MAX_Y_PIXEL){
        return 0;
    }
    else{
        //initialize a temporary buffer, so we don't change the address stored in our double buffer
        char* temp_buffer = NULL;

        //get the number of pixels we have to add to the base address, to draw the pixel we want to draw
        int pixel_offset = (y * SCREEN_RES_X) + x;

        //Initialize the temporary buffer variable with the correct buffer's value
        switch(to_draw_in){
            case b_buf:
                temp_buffer =  (char *) (background_buffer + (pixel_offset * bytes_per_pixel));
                break;
            case d_buf:
                temp_buffer =  (char *) (double_buffer + (pixel_offset * bytes_per_pixel));
                break;
            case vram:
                temp_buffer =  (char *) (video_mem + (pixel_offset * bytes_per_pixel));
                break;
        }

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

//Function which copies the background buffer's contents into the double buffer
void copy_from_background(){
    //copy the memory on the background buffer into the double buffer
    memcpy(double_buffer, background_buffer, vram_size);
}

//Function which copies the double buffer's contents into the background buffer
void copy_to_background(){
    //copy the memory on the double buffer into the background buffer
    memcpy(background_buffer, double_buffer, vram_size);
}

//Function which updates the vram
void update_vram(){
    //copy memory from the buffer into the vram, taking into account the transparency
    memcpy(video_mem, double_buffer, vram_size);
}
