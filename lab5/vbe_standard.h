#ifndef _LCOM_VBE_STANDARD_H_
#define _LCOM_VBE_STANDARD_H_

/*
        For use with VBE Function calls
*/
#define BIOS_INT_10     0x10            //BIOS Interrupt Vector
#define VBE_FUNCTION    0x4F            //Distinguished called function from standard VGA BIOS functions

#define GET_MODE_CONFIG 0x01            //VBE Function 01h
#define SET_VBE_MODE    0x02            //VBE Function 02h
#define LINEAR_BUFFER   BIT(14)         //Used to set the linear frame buffer for a mode

/*
        VBE Function Return Value
*/
#define SUCCESS             0x00
#define FUNC_CALL_FAILED    0x01
#define FUNC_NOT_SUPPORTED  0x02
#define FUNC_INVALID        0x03

/*
        Setting the mode config
*/


#endif /*   _LCOM_VBE_STANDARD_H_  */
