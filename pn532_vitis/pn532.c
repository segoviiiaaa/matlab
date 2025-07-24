#include "pn532.h"
#include <string.h>
#ifdef __BARE_METAL__
#include "xil_printf.h"
#include "sleep.h"
#endif

#define PN532_PREAMBLE       0x00
#define PN532_STARTCODE1     0x00
#define PN532_STARTCODE2     0xFF
#define PN532_POSTAMBLE      0x00

#define PN532_HOSTTOPN532    0xD4
#define PN532_PN532TOHOST    0xD5

#define PN532_COMMAND_SAMCONFIGURATION      0x14
#define PN532_COMMAND_INLISTPASSIVETARGET   0x4A

static int i2c_write(XIicPs *Iic, uint8_t *data, uint32_t len) {
#ifdef __BARE_METAL__
    return XIicPs_MasterSendPolled(Iic, data, len, PN532_I2C_ADDR << 1);
#else
    (void)Iic; /* evitar warning */
    return 0;
#endif
}

static int i2c_read(XIicPs *Iic, uint8_t *data, uint32_t len) {
#ifdef __BARE_METAL__
    return XIicPs_MasterRecvPolled(Iic, data, len, PN532_I2C_ADDR << 1);
#else
    (void)Iic; memset(data,0,len); return 0;
#endif
}

static int wait_ready(XIicPs *Iic, uint32_t timeout_ms) {
#ifdef __BARE_METAL__
    uint8_t status;
    while (timeout_ms--) {
        if (i2c_read(Iic, &status, 1) == 0 && status == 0x01)
            return 0;
        usleep(1000);
    }
    return -1;
#else
    (void)Iic; (void)timeout_ms; return 0;
#endif
}

static int send_command(XIicPs *Iic, uint8_t *cmd, uint8_t cmdlen) {
    uint8_t frame[32];
    uint8_t checksum = PN532_HOSTTOPN532;
    frame[0] = PN532_PREAMBLE;
    frame[1] = PN532_STARTCODE1;
    frame[2] = PN532_STARTCODE2;
    frame[3] = cmdlen + 1;
    frame[4] = (uint8_t)(~frame[3] + 1);
    frame[5] = PN532_HOSTTOPN532;
    for (uint8_t i = 0; i < cmdlen; i++) {
        frame[6+i] = cmd[i];
        checksum += cmd[i];
    }
    frame[6+cmdlen] = (uint8_t)(~checksum + 1);
    frame[7+cmdlen] = PN532_POSTAMBLE;

    return i2c_write(Iic, frame, 8 + cmdlen);
}

int pn532_init(XIicPs *Iic) {
    uint8_t cmd[] = {PN532_COMMAND_SAMCONFIGURATION, 0x01, 0x14, 0x01};
    if (send_command(Iic, cmd, sizeof(cmd)))
        return -1;
    if (wait_ready(Iic, 100))
        return -1;
    uint8_t buf[8];
    if (i2c_read(Iic, buf, sizeof(buf)))
        return -1;
    return 0;
}

int pn532_read_uid(XIicPs *Iic, uint8_t *uid, uint8_t *uid_len) {
    uint8_t cmd[] = {PN532_COMMAND_INLISTPASSIVETARGET, 0x01, 0x00};
    if (send_command(Iic, cmd, sizeof(cmd)))
        return -1;
    if (wait_ready(Iic, 100))
        return -1;
    uint8_t buf[24];
    if (i2c_read(Iic, buf, sizeof(buf)))
        return -1;

    if (buf[6] != PN532_PN532TOHOST || buf[7] != 0x4B || buf[8] != 1)
        return -1;

    *uid_len = buf[12];
    memcpy(uid, &buf[13], *uid_len);
    return 0;
}
