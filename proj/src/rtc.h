#ifndef _RTC_H_
#define _RTC_H_

/** @defgroup rtc rtc
 * @{
 * 
 * Functions for reading and parsing the rtc data
 */

#include <stdint.h>

#define RTC_ADDR_REG  0x70    /*!< the RTC's address register */
#define RTC_DATA_REG  0x71    /*!< the RTC's data register */

#define SECONDS_ADDR  0       /*!< the RTC's second register */
#define MINUTES_ADDR  2       /*!< the RTC's minutes register */
#define HOURS_ADDR    4       /*!< the RTC's hours register */
#define DAY_ADDR      7       /*!< the RTC's days register */
#define MONTH_ADDR    8       /*!< the RTC's months register */
#define REG_A         10      /*!< the RTC's register A */
#define UIP_BIT       BIT(7)  /*!< UIP bit */


/**
 * @brief Enumerated type for specifying which type of data we're reading and saving
 */
typedef enum date{
  sec = 0,    /*!< we're accessing the seconds */
  min = 1,    /*!< we're accessing the minutes */
  hour = 2,   /*!< we're accessing the hours */
  //day = 3,  /*!< we're accessing the days */
  //mon = 4   /*!< we're accessing the months */
} date;

/**
 * @brief Reads data from one register
 * 
 * @param reg Register to read from
 * @param value Where we'll be storing our value
 * @return Returs 0 upon success, non-zero otherwise
 */
int rtc_read_register(uint8_t reg, uint8_t *value);

/**
 * @brief Reads the time
 * 
 * @details Updates the global variable rtc_time with the values read
 * @return Returns 0 upon success, non-zero otherwise
 */
int read_time();

/**
 * @brief Parses the time into an int with the format hhmmdd.
 * 
 * @details This function is useful for saving the highscores later. NOTE: The seconds, minutes and hours read by the rtc are in BCD.
 * @returns Returns int with the time data in the desired format
 */
int parse_time();

#endif /* _RTC_H_ */
