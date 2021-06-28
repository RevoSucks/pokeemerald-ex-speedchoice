EventScript_RepelWoreOff:: @ 82A4B2A
    lockall
	checkitem VAR_LAST_USED_REPEL, 1
	compare VAR_RESULT, TRUE
	goto_if_eq EventScript_AskUseAnother
	msgbox Text_RepelWoreOff, MSGBOX_DEFAULT
    releaseall
	end

EventScript_AskUseAnother:
	msgbox Text_RepelWoreOffUseAnother, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq EventScript_ResetLastUsedRepel
	special Special_UseLastRepelInField
	waitstate
	msgbox gText_PlayerUsedVar2, MSGBOX_DEFAULT
    releaseall
	end

EventScript_ResetLastUsedRepel:
	setvar VAR_LAST_USED_REPEL, ITEM_NONE
    closemessage
    releaseall
	end

Text_RepelWoreOff: @ 82A4B33
	.string "REPEL's effect wore off…$"

Text_RepelWoreOffUseAnother:: @ 81BFB6E
	.string "REPEL's effect wore off…\n"
	.string "Use another?$"
