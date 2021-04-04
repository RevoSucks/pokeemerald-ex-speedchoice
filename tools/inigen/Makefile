CC := gcc
CFLAGS := -g -Wall -iquote ../../include
SRCS := inigen.c elf.c util.c
HEADERS := \
	global.h \
	elf.h \
	util.h \
	../../include/constants/global.h \
    ../../include/constants/species.h \
    ../../include/constants/trainers.h \
    ../../include/constants/items.h \
    ../../include/constants/abilities.h \
    ../../include/constants/moves.h \
    ../../include/constants/pokemon.h \
    ../../include/constants/event_objects.h

LDFLAGS := -lcapstone

.PHONY: all clean

all: inigen
	@:

inigen: $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(SRCS) -o $@ $(LDFLAGS)

clean:
	$(RM) inigen inigen.exe
