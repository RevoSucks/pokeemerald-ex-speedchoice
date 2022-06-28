	.equiv TRACK_SIZE, 0x50
	.equiv NUM_TRACKS_BGM, 16
	.equiv NUM_TRACKS_SE1, 3
	.equiv NUM_TRACKS_SE2, 11
	.equiv NUM_TRACKS_SE3, 1

    .bss

gMPlayTrack_BGM::
    .space TRACK_SIZE * NUM_TRACKS_BGM
    .size gMPlayTrack_BGM, .-gMPlayTrack_BGM

gMPlayTrack_SE1::
    .space TRACK_SIZE * NUM_TRACKS_SE1
    .size gMPlayTrack_SE1, .-gMPlayTrack_SE1

gMPlayTrack_SE2::
    .space TRACK_SIZE * NUM_TRACKS_SE2
    .size gMPlayTrack_SE2, .-gMPlayTrack_SE2

gMPlayTrack_SE3::
    .space TRACK_SIZE * NUM_TRACKS_SE3
    .size gMPlayTrack_SE3, .-gMPlayTrack_SE3

	.section .rodata

	.align 2
gMPlayTable:: @ 86B49C0
	music_player gMPlayInfo_BGM, gMPlayTrack_BGM, 10, 0
	music_player gMPlayInfo_SE1, gMPlayTrack_SE1,  3, 1
	music_player gMPlayInfo_SE2, gMPlayTrack_SE2,  9, 1
	music_player gMPlayInfo_SE3, gMPlayTrack_SE3,  1, 0
