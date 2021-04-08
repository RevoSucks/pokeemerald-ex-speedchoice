#include "global.h"
#include "task.h"
#include "text.h"
#include "scanline_effect.h"
#include "flash_missing_screen.h"
#include "text_window.h"
#include "speedchoice.h"
#include "bg.h"
#include "window.h"
#include "palette.h"
#include "main.h"
#include "gpu_regs.h"
#include "constants/rgb.h"

// Stolen shamelessly from PikalaxALT.

extern const u16 sMainMenuTextPal[16];
extern u16 sOptionMenuText_Pal[16];
extern u16 sOptionMenuBg_Pal[1];

static const u16 sBgPals[] = INCBIN_U16("graphics/interface/save_failed_screen.gbapal");

static const struct BgTemplate sBgTemplates[1] = {
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 2,
        .priority = 0,
        .baseTile = 0x000
    }
};

static const struct WindowTemplate sWindowTemplates[] = {
    {
        0,
        2,
        2,
        26,
        16,
        15,
        10
    },
    DUMMY_WIN_TEMPLATE
};

const u8 sText_FatalError[] = _("   {COLOR RED}{SHADOW LIGHT_RED}No valid backup media was detected.\n    {COLOR BLUE}{SHADOW LIGHT_BLUE}Pokémon Emerald EX{COLOR RED}{SHADOW LIGHT_RED} requires the 1M\n    sub-circuit board to be installed.\n\n           Please turn off the power.\n\n{COLOR DARK_GREY}{SHADOW LIGHT_GREY}mGBA users: Tools {RIGHT_ARROW} Game overrides…\nNOGBA: Options {RIGHT_ARROW} Files Setup\nVBA: Emulator not supported");

static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void Task_FlashMissingScreen(u8 taskId);

void CB2_FlashMissingScreen(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
        DmaClear32(3, (void *)OAM, OAM_SIZE);
        DmaClear16(3, (void *)PLTT, PLTT_SIZE);
        gMain.state++;
        break;
    case 1:
        FillPalette(RGB_BLACK, 0, PLTT_SIZE);
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sBgTemplates, NELEMS(sBgTemplates));
        ChangeBgX(0, 0, 0);
        ChangeBgY(0, 0, 0);
        gMain.state++;
        break;
    case 2:
        ResetPaletteFade();
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        gMain.state++;
        break;
    case 3:
        InitWindows(sWindowTemplates);
        DeactivateAllTextPrinters();
        gMain.state++;
        break;
    case 4:
        LoadBgTiles(0, GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->tiles, 9 * TILE_SIZE_4BPP, 1);
        gMain.state++;
        break;
    case 5:
        LoadPalette(sBgPals, 0, sizeof(sBgPals));
        LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, 0x70, 0x20);
        LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, 0x20, 0x20);
        gMain.state++;
        break;
    case 6:
        LoadPalette(GetTextWindowPalette(2), 0x10, 0x20);
        LoadPalette(sMainMenuTextPal, 0xF0, sizeof(sMainMenuTextPal));
        gMain.state++;
        break;
    case 7:
        SetGpuReg(REG_OFFSET_WIN0H, 0);
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG0);
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 | WINOUT_WIN01_CLR);
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 0);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
        ShowBg(0);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB_BLACK);
        CreateTask(Task_FlashMissingScreen, 0);
        SetMainCallback2(MainCB2);
        SetVBlankCallback(VBlankCB);
        break;
    }
}

static void Task_FlashMissingScreen_Step(u8 taskId);

static void DrawFrame(void)
{
    FillBgTilemapBufferRect(0, 1, 1, 1, 1, 1, 2);
    FillBgTilemapBufferRect(0, 2, 2, 1, 26, 1, 2);
    FillBgTilemapBufferRect(0, 3, 28, 1, 1, 1, 2);
    FillBgTilemapBufferRect(0, 4, 1, 2, 1, 16, 2);
    FillBgTilemapBufferRect(0, 6, 28, 2, 1, 16, 2);
    FillBgTilemapBufferRect(0, 7, 1, 18, 1, 1, 2);
    FillBgTilemapBufferRect(0, 8, 2, 18, 26, 1, 2);
    FillBgTilemapBufferRect(0, 9, 28, 18, 1, 1, 2);
}

static void Task_FlashMissingScreen(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_SLOW;
        FillWindowPixelBuffer(0, PIXEL_FILL(1));
        DrawFrame();
        AddTextPrinterParameterized(0, 2, sText_FatalError, 0, 0, OPTIONS_TEXT_SPEED_SLOW, NULL);
        PutWindowTilemap(0);
        CopyWindowToVram(0, COPYWIN_BOTH);
        gTasks[taskId].func = Task_FlashMissingScreen_Step;
    }
}

static void Task_FlashMissingScreen_Step(u8 taskId)
{

}
