#include "pn532.h"
#ifdef __BARE_METAL__
#include "xparameters.h"
#include "xil_printf.h"
#include "xiicps.h"
#include "sleep.h"
#endif

int main(void) {
XIicPs Iic;
#ifdef __BARE_METAL__
    XIicPs_Config *Config;
    Config = XIicPs_LookupConfig(XPAR_XIICPS_0_DEVICE_ID);
    if (Config == NULL)
        return -1;
    if (XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress) != XST_SUCCESS)
        return -1;
    XIicPs_SetSClk(&Iic, 100000);
#endif

    if (pn532_init(&Iic) != 0) {
#ifdef __BARE_METAL__
        xil_printf("PN532 init failed\n");
#endif
        return -1;
    }

    uint8_t uid[8];
    uint8_t uid_len = 0;
    if (pn532_read_uid(&Iic, uid, &uid_len) != 0) {
#ifdef __BARE_METAL__
        xil_printf("No se detect\u00f3 tarjeta\n");
#endif
        return -1;
    }
#ifdef __BARE_METAL__
    xil_printf("UID:");
    for (uint8_t i = 0; i < uid_len; i++)
        xil_printf(" %02X", uid[i]);
    xil_printf("\n");
#endif
    return 0;
}
