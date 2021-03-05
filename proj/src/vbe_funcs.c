#include <lcom/lcf.h>

#include "vbe_funcs.h"
#include "vbe_standard.h"

int set_graphics_mode(uint16_t mode){
    //initialize struct used to call the function
    struct reg86 r86;

    //zero struct
    memset(&r86, 0, sizeof(r86));

    //BIOS video services
    r86.intno = BIOS_INT_10;

    //set AH register, to distinguish from the standard VGA BIOS functions
    r86.ah = VBE_FUNCTION;

    //the function we're calling is specified in the AL register (set)
    r86.al = SET_VBE_MODE;

    //pass the desired mode in the BX register
    r86.bx = mode;
    //the 14th bit must be 1, to facilitate access to the VRAM (linear framebuffer)
    r86.bx |= LINEAR_BUFFER;

    if( sys_int86(&r86) != 0 ) {
        printf("\tvg_exit(): sys_int86() failed \n");
        return 1;
    }

    //evaluate value saved in the AH register
    switch(r86.ah){
        case SUCCESS:
            //printf("VBE function completed successfully.\n");
            return 0;
        case FUNC_CALL_FAILED:
            printf("Function call failed.\n");
            return 1;
        case FUNC_NOT_SUPPORTED:
            printf("Function is not supported in current HW configuration.\n");
            return 1;
        case FUNC_INVALID:
            printf("Function is invalid in current video mode.\n");
            return 1;
        default:
            printf("Unknown error during the function call.\n");
            return 1;
    }
}

int get_mode_info(uint16_t mode, vbe_mode_info_t *info){

    //Initialized struct that will hold the information related to the mapping of a physical memory region
    mmap_t block;

    //allocate low memory block, and initialize struct with the info
    lm_alloc(sizeof(vbe_mode_info_t), &block);

    //initialize struct used to call the function
    struct reg86 r86;

    //zero struct
    memset(&r86, 0, sizeof(r86));

    //BIOS video services
    r86.intno = BIOS_INT_10;

    //set AH register, to distinguish from the standard VGA BIOS functions
    r86.ah = VBE_FUNCTION;

    //the function we're calling is specified in the AL register (set)
    r86.al = GET_MODE_CONFIG;

    //pass the desired mode in the BX register
    r86.cx = mode;
    r86.cx |= LINEAR_BUFFER;    //set linear buffer

    //pass the base physical address and the physical address offset
    r86.es = PB2BASE(block.phys);
    r86.di = PB2OFF(block.phys);

    if( sys_int86(&r86) != 0 ) {
        printf("\tvg_exit(): sys_int86() failed \n");
        return 1;
    }

    //evaluate value saved in the AH register
    switch(r86.ah){
        case SUCCESS:
            //printf("VBE function completed successfully.\n");
            break;
        case FUNC_CALL_FAILED:
            printf("Function call failed.\n");
            //free memory block previously alocated
            lm_free(&block);
            return 1;
        case FUNC_NOT_SUPPORTED:
            printf("Function is not supported in current HW configuration.\n");
            //free memory block previously alocated
            lm_free(&block);
            return 1;
        case FUNC_INVALID:
            printf("Function is invalid in current video mode.\n");
            //free memory block previously alocated
            lm_free(&block);
            return 1;
        default:
            printf("Unknown error during the function call.\n");
            //free memory block previously alocated
            lm_free(&block);
            return 1;
    }

    //copy struct from the low memory to the struct passed as parameter
    memcpy(info, block.virt, sizeof(vbe_mode_info_t));

    //free memory block previously alocated
    lm_free(&block);

    return 0;
}
