#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  //bitwise and with the lsb (so we end up with 00....00XXXXXXXX)
  val &= (BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0));

  //cast into the appropriate variable
  (*lsb) = (uint8_t) val;

  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  //8 right shifts
  val = val >> 8;

  //cast into the appropriate variable
  (*msb) = (uint8_t) val;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  //create variable and extract value
  uint32_t toParse = 0;
  if( sys_inb(port, &toParse) != 0 ) { return 1; }

  //parse it to where we want to store it
  (*value) = (uint8_t) toParse;

  return 0;
}
