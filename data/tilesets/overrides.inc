	.align 2
gTilesetPalOverrides_General::
	.byte 1             @ palette index
	.byte HOUR_NIGHT    @ start hour (inclusive)
	.byte HOUR_MORNING  @ end hour (exclusive)
	.byte 0             @ padding
	.4byte gTilesetPalOverride_General01

	.byte 5             @ palette index
	.byte HOUR_NIGHT    @ start hour (inclusive)
	.byte HOUR_MORNING  @ end hour (exclusive)
	.byte 0             @ padding
	.4byte gTilesetPalOverride_General05

	.byte 0xFF
	.byte 0
	.byte 0
	.byte 0 @ padding
	.4byte NULL

	.align 2
gTilesetPalOverrides_Petalburg::
	.byte 6             @ palette index
	.byte HOUR_NIGHT    @ start hour (inclusive)
	.byte HOUR_MORNING  @ end hour (exclusive)
	.byte 0             @ padding
	.4byte gTilesetPalOverride_Petalburg06

	.byte 8             @ palette index
	.byte HOUR_NIGHT    @ start hour (inclusive)
	.byte HOUR_MORNING  @ end hour (exclusive)
	.byte 0             @ padding
	.4byte gTilesetPalOverride_Petalburg08

	.byte 0xFF
	.byte 0
	.byte 0
	.byte 0 @ padding
	.4byte NULL
