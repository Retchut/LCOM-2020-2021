#include "lcom/lcf.h"
#include "rtc.h"
#include "utils.h"

//uint8_t rtc_time[5] = {0, 0, 0, 0, 0};
uint8_t rtc_time[3] = {0, 0, 0};

//Function which reads data from a register and stores it in the second argument
int rtc_read_register(uint8_t reg, uint8_t *value){

  if( sys_outb(RTC_ADDR_REG, reg) != 0 ){
    printf("Error writing to the rtc's RTC_ADDR_REG register.\n");
    return 1;
  }

  if( util_sys_inb(RTC_DATA_REG, value) != 0 ){
    printf("Error reading from the rtc's %d register.\n", reg);
    return 1;
  }

  return 0;
}

//Function which reads the time and updates the global variable rtc_time
int read_time(){

  uint8_t state = 1;

  //while the uip bit is on, there's an update in progress
  do{
    if( rtc_read_register(REG_A, &state) != 0 ){
      return 1;
    }
  }while(state & UIP_BIT);

  //read seconds
  if( rtc_read_register(SECONDS_ADDR, &rtc_time[sec]) != 0 ){
    return 1;
  }
  
  //read minutes
  if( rtc_read_register(MINUTES_ADDR, &rtc_time[min]) != 0 ){
    return 1;
  }
  
  //read hours
  if( rtc_read_register(HOURS_ADDR, &rtc_time[hour]) != 0 ){
    return 1;
  }
  
  // //read day
  // if( rtc_read_register(DAY_ADDR, &rtc_time[day]) != 0 ){
  //   return 1;
  // }
  
  // //read month
  // if( rtc_read_register(MONTH_ADDR, &rtc_time[mon]) != 0 ){
  //   return 1;
  // }

  return 0;
}

//Function which parses time into an int so that its digits are hhmmdd
int parse_time(){

  //these variables are in BCD
  //the units are the first 4 bits, and the tens are the last 4 bits
  uint8_t seconds = (rtc_time[sec] >> 4) * 10 + (rtc_time[sec] & 0xF);
  uint8_t minutes = (rtc_time[min] >> 4) * 10 + (rtc_time[min] & 0xF);
  uint8_t hours = (rtc_time[hour] >> 4) * 10 + (rtc_time[hour] & 0xF);

  return hours * 10000 + minutes * 100 + seconds;
}
