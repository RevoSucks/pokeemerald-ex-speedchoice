#include <string.h>
#include "global.h"
#include "task.h"
#include "text.h"
#include "scanline_effect.h"
#include "boot_error_screen.h"
#include "text_window.h"
#include "speedchoice.h"
#include "bg.h"
#include "window.h"
#include "palette.h"
#include "main.h"
#include "gpu_regs.h"
#include "constants/rgb.h"
#include "sound.h"
#include "constants/songs.h"
#include "menu.h"
#include "done_button.h"
#include "AgbAccuracy.h"
#include "string_util.h"

// Stolen shamelessly from PikalaxALT.

EWRAM_DATA u8 gWhichErrorMessage = 0;
EWRAM_DATA u8 gWhichTestFailed[256] = {};

#define TX_MARG_LEFT 2
#define TX_MARG_RIGHT 2
#define TX_MARG_TOP 3
#define TX_WINWID 26

extern const u16 sMainMenuTextPal[16];
extern u16 sOptionMenuText_Pal[16];
extern u16 sOptionMenuBg_Pal[1];

extern void MainCB2_Intro(void);

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

/*
const u8 sText_FatalError[] = _("   {COLOR RED}{SHADOW LIGHT_RED}No valid backup media was detected.\n    {COLOR BLUE}{SHADOW LIGHT_BLUE}Pokémon Emerald EX{COLOR RED}{SHADOW LIGHT_RED} requires the 1M\n    sub-circuit board to be installed.\n\n           Please turn off the power.\n\n{COLOR DARK_GREY}{SHADOW LIGHT_GREY}mGBA users: Tools {RIGHT_ARROW} Game overrides…\nNOGBA: Options {RIGHT_ARROW} Files Setup\nVBA: Emulator not supported");

const u8 sText_BadEmu[] = _("\n       {COLOR RED}{SHADOW LIGHT_RED}The system check has failed.\n    If you are using VBA or a VBA-based\n   emulator, please consider a different\n               more modern emulator.\n\n   If you are on console, please contact\n                 the ROM developers.");
*/

static const u8 sText_BootError_1[] = _("No valid backup media was detected.");
static const u8 sText_BootError_2[] = _("Pokémon Emerald EX{COLOR RED}{SHADOW LIGHT_RED} requires the 1M");
static const u8 sText_BootError_3[] = _("sub-circuit board to be installed.");
static const u8 sText_BootError_5[] = _("Please turn off the power.");
static const u8 sText_BootError_7[] = _("{COLOR DARK_GREY}{SHADOW LIGHT_GREY}mGBA: Tools {RIGHT_ARROW} Game overrides…");
static const u8 sText_BootError_8[] = _("NOGBA: Options {RIGHT_ARROW} Files Setup");
static const u8 sText_BootError_9[] = _("VBA: Emulator not supported");

static const u8 sText_PipelineFail_1[] = _("Emulation accuracy test failed.");
static const u8 sText_PipelineFail_2[] = _("The game can still be played, but");
static const u8 sText_PipelineFail_3[] = _("bug reports will be ignored.");
static const u8 sText_PipelineFail_4[] = _("FAILED TEST(S):");

static const u8 sText_PressAnyKeyToContinue[] = _("Press Button A or B to continue.");

struct FatalErrorText
{
    u8 alignment;
    u8 color;
    const u8 * str;
};

struct FatalErrorCnt
{
    bool8 isFatal;
    struct FatalErrorText texts[9];
};

enum {
    FMS_COLOR_GREY = 0,
    FMS_COLOR_RED,
    FMS_COLOR_BLUE,
    FMS_COLOR_GREEN,
};

enum {
    TEXT_LEFT,
    TEXT_CENTER,
    TEXT_RIGHT
};

static const u8 sTextColors[][3] = {
    [FMS_COLOR_GREY] = { TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GREY, TEXT_COLOR_LIGHT_GREY },
    [FMS_COLOR_RED] = { TEXT_COLOR_WHITE, TEXT_COLOR_RED, TEXT_COLOR_LIGHT_RED },
    [FMS_COLOR_BLUE] = { TEXT_COLOR_WHITE, TEXT_COLOR_BLUE, TEXT_COLOR_LIGHT_BLUE },
    [FMS_COLOR_GREEN] = { TEXT_COLOR_WHITE, TEXT_COLOR_GREEN, TEXT_COLOR_LIGHT_GREEN },
};

static const struct FatalErrorCnt sTexts_FatalError[] = {
    [FATAL_NO_FLASH] = {
        TRUE,
        {
            {TEXT_CENTER, FMS_COLOR_RED, sText_BootError_1},
            {TEXT_CENTER, FMS_COLOR_BLUE, sText_BootError_2},
            {TEXT_CENTER, FMS_COLOR_RED, sText_BootError_3},
            {},
            {TEXT_CENTER, FMS_COLOR_RED, sText_BootError_5},
            {},
            {TEXT_LEFT, FMS_COLOR_GREY, sText_BootError_7},
            {TEXT_LEFT, FMS_COLOR_GREY, sText_BootError_8},
            {TEXT_LEFT, FMS_COLOR_GREY, sText_BootError_9}
        }
    },
    [FATAL_ACCU_FAIL] = {
        FALSE,
        {
            {TEXT_CENTER, FMS_COLOR_RED, sText_PipelineFail_1},
            {TEXT_CENTER, FMS_COLOR_RED, sText_PipelineFail_2},
            {TEXT_CENTER, FMS_COLOR_RED, sText_PipelineFail_3},
            {TEXT_LEFT, FMS_COLOR_GREY, sText_PipelineFail_4},
            {TEXT_LEFT, FMS_COLOR_BLUE, gStringVar4}
        }
    }
};

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

static void Task_BootErrorScreen(u8 taskId);

void CB2_BootErrorScreen(void)
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
        CreateTask(Task_BootErrorScreen, 0);
        SetMainCallback2(MainCB2);
        SetVBlankCallback(VBlankCB);
        break;
    }
}

static void Task_BootErrorScreen_Step(u8 taskId);

extern u64 gAgbAccuracyResult;

char c2h(char c) {
  switch (c) {
  case '0' ... '9':
    return c - '0';
  case 'A' ... 'F':
    return c + 10 - 'A';
  case 'a' ... 'f':
    return c + 10 - 'a';
  default:
    return 0xFF;
  }
}

// Input string is assumed ASCII.
u8 * ascii2gf(u8 *dest, const char *src) {
  char c;
  while ((c = *src++)) {
    switch (c) {
    case '0' ... '9':
      *dest++ = c - '0' + CHAR_0;
      break;
    case 'A' ... 'Z':
      *dest++ = c - 'A' + CHAR_A;
      break;
    case 'a' ... 'z':
      *dest++ = c - 'a' + CHAR_a;
      break;
    // more cases for punctuation
    case '{':
      while (*src != '}') {
        *dest = c2h(*src++) << 4;
        *dest |= c2h(*src++);
        while (*src == ' ') src++;
        dest++;
      }
      src++;
      break;
    default:
      *dest++ = CHAR_SPACE;
      break;
    }
  }
  *dest++ = EOS;
  return dest;
}

void BufferWhichErrorsText(void)
{
    u8 i;
    int c;
    u8 *ptr;
    u8 buffer[32];
    u8 j;
    gStringVar4[0] = EOS;
    ptr = gStringVar4;
    
    for(i = 0, c = 0; i < 64; i++)
    {
        // if this is set, it means the cooresponding gTestSpecs element returned
        // an error. Buffer the string after converting it from ASCII.
        if (gAgbAccuracyResult & (1ULL << i))
        {
            ascii2gf(buffer, gTestSpecs[i].name);
            if (c % 2)
            {
                s32 x = 204 - GetStringWidth(1, buffer, 1);
                *ptr++ = EXT_CTRL_CODE_BEGIN;
                *ptr++ = EXT_CTRL_CODE_CLEAR_TO;
                *ptr++ = x;
                ptr = StringCopy(ptr, buffer);
                *ptr++ = CHAR_NEWLINE;
                *ptr = EOS;
            }
            else
            {
                ptr = StringCopy(ptr, buffer);
            }
            c++;
        }
    }
}

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

static s32 GetStringXpos(const u8 * str, u8 mode)
{
    switch (mode)
    {
    case TEXT_LEFT:
    default:
        return TX_MARG_RIGHT;
    case TEXT_CENTER:
        return (((TX_WINWID) * 8 - (TX_MARG_LEFT) - (TX_MARG_RIGHT)) - GetStringWidth(2, str, 1)) / 2 + TX_MARG_LEFT;
    case TEXT_RIGHT:
        return ((TX_WINWID) * 8 - (TX_MARG_RIGHT)) - GetStringWidth(2, str, 1);
    }
}

static void Task_BootErrorScreen(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        s32 i;
        FillWindowPixelBuffer(0, PIXEL_FILL(1));
        DrawFrame();
        BufferWhichErrorsText();
        for (i = 0; i < 9; i++)
        {
            if (sTexts_FatalError[gWhichErrorMessage].texts[i].str != NULL)
                AddTextPrinterParameterized3(
                    0,
                    2,
                    GetStringXpos(sTexts_FatalError[gWhichErrorMessage].texts[i].str, sTexts_FatalError[gWhichErrorMessage].texts[i].alignment),
                    14 * i + TX_MARG_TOP,
                    sTextColors[sTexts_FatalError[gWhichErrorMessage].texts[i].color],
                    TEXT_SPEED_FF,
                    sTexts_FatalError[gWhichErrorMessage].texts[i].str
                );
        }
        PutWindowTilemap(0);
        CopyWindowToVram(0, COPYWIN_BOTH);
        gTasks[taskId].func = Task_BootErrorScreen_Step;
        gTasks[taskId].data[0] = 0;
    }
}

static void Task_WaitButton(u8 taskId);
static void Task_BeginFadeOut(u8 taskId);
static void Task_WaitFadeOut(u8 taskId);

static void Task_BootErrorScreen_Step(u8 taskId)
{
    switch (gTasks[taskId].data[0]++)
    {
    case 0:
    case 30:
        PlaySE(SE_BOO);
        break;
    case 60:
        PlaySE(SE_BOO);
        if (sTexts_FatalError[gWhichErrorMessage].isFatal)
            DestroyTask(taskId);
        break;
    case 180:
        AddTextPrinterParameterized3(
            0,
            2,
            GetStringXpos(sText_PressAnyKeyToContinue, TEXT_CENTER),
            14 * 8 + TX_MARG_TOP,
            sTextColors[FMS_COLOR_GREY],
            TEXT_SPEED_FF,
            sText_PressAnyKeyToContinue
        );
        PutWindowTilemap(0);
        CopyWindowToVram(0, COPYWIN_BOTH);
        gTasks[taskId].func = Task_WaitButton;
        break;
    }
}

static void Task_WaitButton(u8 taskId)
{
    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].data[0] = 60;
        gTasks[taskId].func = Task_BeginFadeOut;
    }
}

static void Task_BeginFadeOut(u8 taskId)
{
    if (--gTasks[taskId].data[0] == 0)
    {
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_WaitFadeOut;
    }
}

void Task_Scene1_Load(u8 taskId);

static void Task_WaitFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(MainCB2_Intro);
        CreateTask(Task_Scene1_Load, 0);
        sInIntro = TRUE;
        DestroyTask(taskId);
    }
}
