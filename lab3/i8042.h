#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#define TIMEOUT         5
#define TIMED_OUT   -1

#define KBD_IRQ    1
#define MS_IRQ     12
#define CTRL_REG   0x64
#define STATUS_REG 0x64

#define IN_BUF     0x60         //to send command parameters to
#define OUT_BUF    0x60         //to read data from

#define DELAY_US   20000


/*
        KEYBOARD COMMANDS
*/

#define READ_CMD 0x20
#define WRITE_CMD 0x60


/*
        MOUSE COMMANDS
*/

#define WRITE_BYTE_TO_MOUSE 0xD4
#define ENABLE_DATA_REPORTING 0xF4
#define DISABLE_DATA_REPORTING 0xF5

/*
        MOUSE ACKNOWLEDGMENTS
*/

#define ACK     0xFA    //everything's ok
#define NACK    0xFE    //negative acknowledgment - write command again
#define ERROR   0xFC    //error - sending command failed

/*
        KEYBOARD AND MOUSE CONTROLLER STATUS REGISTER BYTES
*/

//Output buffer status (0 if empty, don't read; 1 if full, can be read)
#define OB_STATUS BIT(0)

//Input buffer status (0 if empty, can be written; 1 if full, don't write)
#define IB_STATUS BIT(1)

//System flag. 0 after power on reset, 1 after successful completion of the keyboard controller self test
#define SYS_FLAG BIT(2)

//0 if the last write to input buffer was data; 1 if it was a command
#define COMMAND_DATA BIT(3)

//0 if locked; 1 if unlocked
#define KBD_LOCK BIT(4)

//If a read from port 0 is valid (check bit 0), this bit tells what data will be read from port 0x60 (0 - keyboard data; 1 - mouse data) - ON PS/2 SYSTEMS
#define AUX_OUT_BUF_FULL BIT(5)

// 0 if ok, 1 if general timeout (PS/2 SYSTEMS) (Timeout means invalid data for some reason. No idea why)
#define TIMEOUT_ERROR BIT(6)

//0 if ok, 1 if parity error with last byte (Also means invalid data, apparently. No idea what the parity error actually is tho)
#define PARITY_ERROR BIT(7)

/*
        KEYBOARD CONTROLLER COMMAND BYTE
*/

//0 - do not use keyboard interrupts; 1 - send interrupt request IRQ1, when the keyboard output buffer is full
#define KBD_INT_ENABLE BIT(0)

//0 - do not use mouse interrupts (PS/2) system; 1 - send interrupt request IRQ12 when the mouse output buffer is full
#define MS_INT_ENABLE BIT(1)

//cold reboot - reboot with the bit set to 0; warm reboot - this set to 1
#define SYS_FLAG BIT(2)

//for PS/2, unused (always 0)
#define IGN_KBD_LOCK BIT(3)

//if 0, enable keyboard; if 1, disable keyboard (by driving the clockline low)
#define KBD_ENABLE BIT(4)

//if 0, enable mouse; if 1, use 8086 codes, don't check parity and don't do scan conversion
#define MS_ENABLE BIT(5)

//if 0, no translation; if 1, translate keyboard scancodes
#define KBD_TRANSLATE BIT(6)

//bit 7 is unused, always 0

#endif /* _LCOM_I8042_H_ */

/*
        MOUSE PACKET FORMAT:
                BYTE 1: DESCRIBED IN THE DEFINES BELOW
                        BYTE 1'S BIT 3 IS ALWAYS 1
                BYTE 2: X MOVEMENT*
                BYTE 3: Y MOVEMENT*
        
        * THE X AND Y MOVEMENT ARE 9 BIT 2s COMPLEMENT VALUES
        WHERE THE MSB IS THE RESPECTIVE SIGN BIT ON BYTE 1
*/

//1 if the Left Mouse Button was pressed
#define LMB_CLICK       BIT(0)

//1 if the Right Mouse Button was pressed
#define RMB_CLICK       BIT(1)

//1 if the Middle Mouse Button was pressed
#define MMB_CLICK       BIT(2)

//X movement's sign bit
#define X_SIGN          BIT(4)

//Y movement's sign bit
#define Y_SIGN          BIT(5)

//If X overflow occurred
#define X_OVERFLOW      BIT(6)

//If Y overflow ocurred
#define Y_OVERFLOW      BIT(7)
