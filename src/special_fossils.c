#include "global.h"
#include "malloc.h"
#include "constants/items.h"
#include "list_menu.h"
#include "task.h"
#include "item.h"
#include "text.h"
#include "strings.h"
#include "script.h"
#include "event_data.h"
#include "menu.h"

static const u16 sFossils[] = {
    ITEM_HELIX_FOSSIL,
    ITEM_DOME_FOSSIL,
    ITEM_OLD_AMBER,
    ITEM_ROOT_FOSSIL,
    ITEM_CLAW_FOSSIL,
    ITEM_ARMOR_FOSSIL,
    ITEM_SKULL_FOSSIL,
    ITEM_COVER_FOSSIL,
    ITEM_PLUME_FOSSIL,
    ITEM_JAW_FOSSIL,
    ITEM_SAIL_FOSSIL,
};

#define tState         data[0]
#define tListId        data[1]
#define tWindowId      data[2]
#define tCursorPos     data[3]
#define tItemsAbove    data[4]
#define tItemsOffs     14

static void Task_AskFossils(u8 taskId)
{
    struct ListMenuItem * listMenuItems;
    s32 i;
    s32 nitems;
    s32 maxShowed;
    s32 input;
    struct WindowTemplate windowTemplate;

    switch (gTasks[taskId].tState)
    {
    case 0:
        listMenuItems = AllocZeroed((NELEMS(sFossils) + 1) * sizeof(struct ListMenuItem));
        SetWordTaskArg(taskId, tItemsOffs, (uintptr_t)listMenuItems);

        for (i = 0, nitems = 0; i < NELEMS(sFossils); i++)
        {
            if (CheckBagHasItem(sFossils[i], 1) == TRUE)
            {
                listMenuItems[nitems].id = sFossils[i];
                listMenuItems[nitems].name = ItemId_GetName(sFossils[i]);
                nitems++;
            }
        }
        listMenuItems[nitems].id = LIST_CANCEL;
        listMenuItems[nitems].name = gText_Cancel;
        nitems++;

        maxShowed = min(nitems, 5);
        windowTemplate = (struct WindowTemplate){
            .bg = 0,
            .tilemapLeft = 19,
            .tilemapTop = 2,
            .width = 9,
            .paletteNum = 15,
            .baseBlock = 0x21D
        };
        gMultiuseListMenuTemplate = (struct ListMenuTemplate) {
            .moveCursorFunc = ListMenuDefaultCursorMoveFunc,
            .itemPrintFunc = NULL,
            .header_X = 0,
            .item_X = 8,
            .cursor_X = 0,
            .upText_Y = 2,
            .cursorPal = TEXT_COLOR_DARK_GRAY,
            .fillValue = TEXT_COLOR_WHITE,
            .cursorShadowPal = TEXT_COLOR_LIGHT_GRAY,
            .lettersSpacing = 1,
            .itemVerticalPadding = 0,
            .scrollMultiple = LIST_NO_MULTIPLE_SCROLL,
            .fontId = 1,
            .cursorKind = 0,
        };
        windowTemplate.height = 2 * maxShowed + 1;
        gTasks[taskId].tWindowId = AddWindow(&windowTemplate);
        FillWindowPixelBuffer(gTasks[taskId].tWindowId, PIXEL_FILL(0));
        DrawStdFrameWithCustomTileAndPalette(gTasks[taskId].tWindowId, FALSE, 0x214, 14);
        gMultiuseListMenuTemplate.items = listMenuItems;
        gMultiuseListMenuTemplate.totalItems = nitems;
        gMultiuseListMenuTemplate.maxShowed = maxShowed;
        gMultiuseListMenuTemplate.windowId = gTasks[taskId].tWindowId;
        gTasks[taskId].tListId = ListMenuInit(&gMultiuseListMenuTemplate, 0, 0);
        PutWindowTilemap(gTasks[taskId].tWindowId);
        CopyWindowToVram(gTasks[taskId].tWindowId, 1);
        gTasks[taskId].tState++;
        break;
    case 1:
        input = ListMenu_ProcessInput(gTasks[taskId].tListId);
        if (input != LIST_NOTHING_CHOSEN)
        {
            DestroyListMenuTask(gTasks[taskId].tListId, (u16 *)&gTasks[taskId].tCursorPos, (u16 *)&gTasks[taskId].tItemsAbove);
            listMenuItems = (struct ListMenuItem *)GetWordTaskArg(taskId, tItemsOffs);
            Free(listMenuItems);
            gSpecialVar_Result = input;
            ClearStdWindowAndFrameToTransparent(gTasks[taskId].tWindowId, TRUE);
            RemoveWindow(gTasks[taskId].tWindowId);
            DestroyTask(taskId);
            EnableBothScriptContexts();
        }
        break;
    }
}

void Special_AskFossils(void)
{
    CreateTask(Task_AskFossils, 10);
}
