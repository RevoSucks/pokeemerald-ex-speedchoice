.set LOCALID_NURSE, 1
.set LOCALID_GUARD_1, 3
.set LOCALID_GUARD_2, 4

EverGrandeCity_PokemonLeague_1F_MapScripts:: @ 82295D2
	map_script MAP_SCRIPT_ON_TRANSITION, EverGrandeCity_PokemonLeague_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

EverGrandeCity_PokemonLeague_1F_OnTransition: @ 82295DD
	setrespawn HEAL_LOCATION_EVER_GRANDE_CITY_POKEMON_LEAGUE
	setflag FLAG_LANDMARK_POKEMON_LEAGUE
	call_if_unset FLAG_ENTERED_ELITE_FOUR, EverGrandeCity_PokemonLeague_1F_EventScript_GuardsBlockDoor
	end

EverGrandeCity_PokemonLeague_1F_EventScript_GuardsBlockDoor:: @ 82295ED
	setobjectxyperm LOCALID_GUARD_1, 9, 2
	setobjectxyperm LOCALID_GUARD_2, 10, 2
	return

EverGrandeCity_PokemonLeague_1F_EventScript_Nurse:: @ 82295FC
	setvar VAR_0x800B, LOCALID_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

EverGrandeCity_PokemonLeague_1F_EventScript_Clerk:: @ 822960A
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart EverGrandeCity_PokemonLeague_1F_Pokemart
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

EverGrandeCity_PokemonLeague_1F_EventScript_Clerk2::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart EverGrandeCity_PokemonLeague_1F_Pokemart2
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

EverGrandeCity_PokemonLeague_1F_EventScript_Clerk3::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart EverGrandeCity_PokemonLeague_1F_Pokemart3
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
EverGrandeCity_PokemonLeague_1F_Pokemart:
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_MAX_POTION
	.2byte ITEM_FULL_RESTORE
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_REVIVE
	.2byte ITEM_MAX_REPEL
	.2byte ITEM_NONE
	release
	end

	.align 2
EverGrandeCity_PokemonLeague_1F_Pokemart2:
	.2byte ITEM_ABOMASITE
	.2byte ITEM_ABSOLITE
	.2byte ITEM_AERODACTYLITE
	.2byte ITEM_AGGRONITE
	.2byte ITEM_ALAKAZITE
	.2byte ITEM_ALTARIANITE
	.2byte ITEM_AMPHAROSITE
	.2byte ITEM_AUDINITE
	.2byte ITEM_BANETTITE
	.2byte ITEM_BEEDRILLITE
	.2byte ITEM_BLASTOISINITE
	.2byte ITEM_BLAZIKENITE
	.2byte ITEM_CAMERUPTITE
	.2byte ITEM_CHARIZARDITE_X
	.2byte ITEM_CHARIZARDITE_Y
	.2byte ITEM_DIANCITE
	.2byte ITEM_GALLADITE
	.2byte ITEM_GARCHOMPITE
	.2byte ITEM_GARDEVOIRITE
	.2byte ITEM_GENGARITE
	.2byte ITEM_GLALITITE
	.2byte ITEM_GYARADOSITE
	.2byte ITEM_HERACRONITE
	.2byte ITEM_HOUNDOOMINITE
	.2byte ITEM_KANGASKHANITE
	.2byte ITEM_LATIASITE
	.2byte ITEM_LATIOSITE
	.2byte ITEM_LOPUNNITE
	.2byte ITEM_LUCARIONITE
	.2byte ITEM_MANECTITE
	.2byte ITEM_MAWILITE
	.2byte ITEM_MEDICHAMITE
	.2byte ITEM_METAGROSSITE
	.2byte ITEM_MEWTWONITE_X
	.2byte ITEM_MEWTWONITE_Y
	.2byte ITEM_PIDGEOTITE
	.2byte ITEM_PINSIRITE
	.2byte ITEM_SABLENITE
	.2byte ITEM_SALAMENCITE
	.2byte ITEM_SCEPTILITE
	.2byte ITEM_SCIZORITE
	.2byte ITEM_SHARPEDONITE
	.2byte ITEM_SLOWBRONITE
	.2byte ITEM_STEELIXITE
	.2byte ITEM_SWAMPERTITE
	.2byte ITEM_TYRANITARITE
	.2byte ITEM_VENUSAURITE
    .2byte ITEM_NONE
	release
	end

	.align 2
EverGrandeCity_PokemonLeague_1F_Pokemart3:
	.2byte ITEM_NORMALIUM_Z
	.2byte ITEM_FIGHTINIUM_Z
	.2byte ITEM_FLYINIUM_Z
	.2byte ITEM_POISONIUM_Z
	.2byte ITEM_GROUNDIUM_Z
	.2byte ITEM_ROCKIUM_Z
	.2byte ITEM_BUGINIUM_Z
	.2byte ITEM_GHOSTIUM_Z
	.2byte ITEM_STEELIUM_Z
	.2byte ITEM_FIRIUM_Z
	.2byte ITEM_WATERIUM_Z
	.2byte ITEM_GRASSIUM_Z
	.2byte ITEM_ELECTRIUM_Z
	.2byte ITEM_PSYCHIUM_Z
	.2byte ITEM_ICIUM_Z
	.2byte ITEM_DRAGONIUM_Z
	.2byte ITEM_DARKINIUM_Z
	.2byte ITEM_FAIRIUM_Z
	.2byte ITEM_ALORAICHIUM_Z
	.2byte ITEM_DECIDIUM_Z
	.2byte ITEM_EEVIUM_Z
	.2byte ITEM_INCINIUM_Z
	.2byte ITEM_KOMMONIUM_Z
	.2byte ITEM_LUNALIUM_Z
	.2byte ITEM_LYCANIUM_Z
	.2byte ITEM_MARSHADIUM_Z
	.2byte ITEM_MEWNIUM_Z
	.2byte ITEM_MIMIKIUM_Z
	.2byte ITEM_PIKANIUM_Z
	.2byte ITEM_PIKASHUNIUM_Z
	.2byte ITEM_PRIMARIUM_Z
	.2byte ITEM_SNORLIUM_Z
	.2byte ITEM_SOLGANIUM_Z
	.2byte ITEM_TAPUNIUM_Z
	.2byte ITEM_ULTRANECROZIUM_Z
    .2byte ITEM_NONE
	release
	end

@ The door guards only check for FLAG_BADGE06_GET because Winonas badge is the only one that can be skipped
@ Its assumed the player has the remaining badges
EverGrandeCity_PokemonLeague_1F_EventScript_DoorGuard:: @ 8229636
	lockall
	goto_if_set FLAG_ENTERED_ELITE_FOUR, EverGrandeCity_PokemonLeague_1F_EventScript_GoForth
	getplayerxy VAR_TEMP_0, VAR_TEMP_1
	compare VAR_TEMP_0, 11
	call_if_ge EverGrandeCity_PokemonLeague_1F_EventScript_PlayerMoveToFrontFromRight
	compare VAR_TEMP_0, 8
	call_if_le EverGrandeCity_PokemonLeague_1F_EventScript_PlayerMoveToFrontFromLeft
	message EverGrandeCity_PokemonLeague_1F_Text_MustHaveAllGymBadges
	waitmessage
	delay 120
	goto_if_unset FLAG_BADGE06_GET, EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges
	closemessage
	applymovement LOCALID_GUARD_1, EverGrandeCity_PokemonLeague_1F_Movement_LeftGuardOutOfWay
	applymovement LOCALID_GUARD_2, EverGrandeCity_PokemonLeague_1F_Movement_RightGuardOutOfWay
	waitmovement 0
	delay 10
	playfanfare MUS_OBTAIN_BADGE
	message EverGrandeCity_PokemonLeague_1F_Text_GoForth
	waitmessage
	waitfanfare
	closemessage
	copyobjectxytoperm LOCALID_GUARD_1
	copyobjectxytoperm LOCALID_GUARD_2
	setflag FLAG_ENTERED_ELITE_FOUR
	releaseall
	end

EverGrandeCity_PokemonLeague_1F_EventScript_PlayerMoveToFrontFromRight:: @ 8229698
	applymovement OBJ_EVENT_ID_PLAYER, EverGrandeCity_PokemonLeague_1F_Movement_MoveToFrontFromRight
	waitmovement 0
	return

EverGrandeCity_PokemonLeague_1F_EventScript_PlayerMoveToFrontFromLeft:: @ 82296A3
	applymovement OBJ_EVENT_ID_PLAYER, EverGrandeCity_PokemonLeague_1F_Movement_MoveToFrontFromLeft
	waitmovement 0
	return

EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges:: @ 82296AE
	playse SE_FAILURE
	msgbox EverGrandeCity_PokemonLeague_1F_Text_HaventObtainedAllBadges, MSGBOX_DEFAULT
	releaseall
	end

EverGrandeCity_PokemonLeague_1F_EventScript_GoForth:: @ 82296BB
	applymovement VAR_LAST_TALKED, Common_Movement_FacePlayer
	waitmovement 0
	msgbox EverGrandeCity_PokemonLeague_1F_Text_GoForth, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

EverGrandeCity_PokemonLeague_1F_Movement_MoveToFrontFromRight: @ 82296DA
	walk_down
	walk_left
	walk_in_place_fastest_up
	step_end

EverGrandeCity_PokemonLeague_1F_Movement_MoveToFrontFromLeft: @ 82296DE
	walk_down
	walk_right
	walk_in_place_fastest_up
	step_end

EverGrandeCity_PokemonLeague_1F_Movement_LeftGuardOutOfWay: @ 82296E2
	walk_left
	walk_in_place_fastest_down
	step_end

EverGrandeCity_PokemonLeague_1F_Movement_RightGuardOutOfWay: @ 82296E5
	walk_right
	walk_in_place_fastest_down
	step_end

EverGrandeCity_PokemonLeague_1F_Text_MustHaveAllGymBadges: @ 82296E8
	.string "Beyond this point, only those TRAINERS\n"
	.string "who have collected all the GYM BADGES\l"
	.string "are permitted to enter.\p"
	.string "TRAINER, let us confirm that you have\n"
	.string "all the GYM BADGES.$"

EverGrandeCity_PokemonLeague_1F_Text_HaventObtainedAllBadges: @ 8229787
	.string "You haven't obtained all the BADGES.\p"
	.string "If you're bound for the POKéMON\n"
	.string "LEAGUE, you must return with them.$"

EverGrandeCity_PokemonLeague_1F_Text_GoForth: @ 82297EF
	.string "TRAINER! Believe in yourself and your\n"
	.string "POKéMON, and go forth!$"

