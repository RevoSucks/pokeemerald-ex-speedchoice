#ifndef POKEEMEREX_BOOT_ERROR_SCREEN_H
#define POKEEMEREX_BOOT_ERROR_SCREEN_H

enum {
    FATAL_NO_FLASH = 0,
    FATAL_ACCU_FAIL,
    FATAL_OKAY = 255,
};

extern int gErrorScreenCode;

void CB2_BootErrorScreen(void);
extern u8 gWhichErrorMessage;
extern u8 gWhichTestFailed[];

#endif //POKEEMEREX_BOOT_ERROR_SCREEN_H
