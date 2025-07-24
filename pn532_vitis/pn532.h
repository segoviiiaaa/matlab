#ifndef PN532_H
#define PN532_H

#include <stdint.h>

#ifdef __BARE_METAL__
#include "xiicps.h"
#else
/* Compilaci\u00f3n sin Vitis para verificar sintaxis */
typedef struct { int dummy; } XIicPs;
#endif

#define PN532_I2C_ADDR    0x24 /* Direcci\u00f3n I2C de 7 bits */

int pn532_init(XIicPs *Iic);
int pn532_read_uid(XIicPs *Iic, uint8_t *uid, uint8_t *uid_len);

#endif /* PN532_H */
