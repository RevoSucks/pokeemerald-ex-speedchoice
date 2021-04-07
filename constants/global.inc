	.set TRUE, 1
	.set FALSE, 0

	.set NULL, 0

	.set SPRITE_SIZE_8x8,   (OAM_SIZE_0 >> 28) | (OAM_SQUARE >> 14)
	.set SPRITE_SIZE_16x16, (OAM_SIZE_1 >> 28) | (OAM_SQUARE >> 14)
	.set SPRITE_SIZE_32x32, (OAM_SIZE_2 >> 28) | (OAM_SQUARE >> 14)
	.set SPRITE_SIZE_64x64, (OAM_SIZE_3 >> 28) | (OAM_SQUARE >> 14)

	.set SPRITE_SIZE_16x8,  (OAM_SIZE_0 >> 28) | (OAM_H_RECTANGLE >> 14)
	.set SPRITE_SIZE_32x8,  (OAM_SIZE_1 >> 28) | (OAM_H_RECTANGLE >> 14)
	.set SPRITE_SIZE_32x16, (OAM_SIZE_2 >> 28) | (OAM_H_RECTANGLE >> 14)
	.set SPRITE_SIZE_64x32, (OAM_SIZE_3 >> 28) | (OAM_H_RECTANGLE >> 14)

	.set SPRITE_SIZE_8x16,  (OAM_SIZE_0 >> 28) | (OAM_V_RECTANGLE >> 14)
	.set SPRITE_SIZE_8x32,  (OAM_SIZE_1 >> 28) | (OAM_V_RECTANGLE >> 14)
	.set SPRITE_SIZE_16x32, (OAM_SIZE_2 >> 28) | (OAM_V_RECTANGLE >> 14)
	.set SPRITE_SIZE_32x64, (OAM_SIZE_3 >> 28) | (OAM_V_RECTANGLE >> 14)

	.set OBJ_IMAGE_ANIM_H_FLIP, 1 << 6
	.set OBJ_IMAGE_ANIM_V_FLIP, 1 << 7

    @ README: ensure that these values are aligned with the enum in speedchoice.h!
    @ 0 is for the preset value in the enum and 1 is for the player name.
    @ DO NOT USE THESE VALUES!
    @ 0 reserved for PRESET
    @ 1 reserved for PLAYER_NAME
	.set EXPMATH, 2
	.set PLOTLESS, 3
	.set SPINNERS, 4
	.set MAXVISION, 5
	.set EARLYFLY, 6
	.set GOOD_EARLY_WILDS, 7
	.set EARLYSURF, 8
	.set NICE_MENU_ORDER, 9
	.set EASY_FALSE_SWIPE, 10
    .set FAST_CATCH, 11
    .set GEN_7_X_ITEMS, 12
    .set EVO_EVERY_LEVEL, 13

    @ -------------
    @ option enums
    @ -------------

    @ EXPMATH
    .set EXP_KEEP, 0
    .set EXP_BW, 1
    .set EXP_NONE, 2

    @ PLOTLESS
    .set PLOT_SEMI, 0
    .set PLOT_KEEP, 1
    .set PLOT_FULL, 2

    @ INSTANT TEXT (DEPRECATED)
    .set IT_ON, 0
    .set IT_OFF, 1

    @ SPINNERS
    .set SPIN_NERF, 0
    .set SPIN_KEEP, 1
    .set SPIN_HELL, 2
    .set SPIN_WHY, 3

    @ MAX VISION
    .set MAX_OFF, 0
    .set MAX_SANE, 1
    .set MAX_HELL, 2

    @ NERF ROXANNE (DEPRECATED)
    .set NERF_YES, 0
    .set NERF_NO, 1

    @ SUPER BIKE (DEPRECATED)
    .set BIKE_ON, 0
    .set BIKE_OFF, 1

    @ NEW WILD ENC (DEPRECATED)
    .set NEW_ON, 0
    .set NEW_OFF, 1

    @ EARLY FLY
    .set FLY_YES, 0
    .set FLY_NO, 1

    @ RUN EVERYWHERE (DEPRECATED)
    .set RUN_ON, 0
    .set RUN_OFF, 1

    @ MEME ISLAND (DEPRECATED)
    .set MEME_BIG, 0 @ dot net
    .set MEME_SMALL, 1 @ you're banned

    @ BETTER MARTS (DEPRECATED)
    .set MARTS_ON, 0
    .set MARTS_OFF, 1

    @ GOOD EARLY WILDS
    .set GOOD_OFF, 0
    .set GOOD_STATIC, 1
    .set GOOD_RAND, 2

    @ EARLY SURF
    .set SURF_ON, 0
    .set SURF_OFF, 1

    @ NICE_MENU_ORDER
    .set NICE_MENU_ORDER_ON, 0
    .set NICE_MENU_ORDER_OFF, 1

    @ EASY_FALSE_SWIPE
    .set EASY_FALSE_SWIPE_OFF, 0
    .set EASY_FALSE_SWIPE_TUTOR, 1
    .set EASY_FALSE_SWIPE_HM05, 2
