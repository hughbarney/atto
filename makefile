#
# makefile
#
# Based on Anthonys Editor January 93
#
# Public Domain 1991, 1993 by Anthony Howe.  No warranty.
#

BINDIR  = /usr/local/bin
DOCDIR  = /usr/local/share/doc/atto
DESTDIR =

CC      = cc
CFLAGS  = -O -Wall

LD      = cc
LDFLAGS =
LIBS    = -lncursesw

CP      = cp
MV      = mv
RM      = rm
MKDIR	= mkdir

E       =
O       = .o

OBJ     = command$(O) display$(O) gap$(O) key$(O) search$(O) buffer$(O) replace$(O) window$(O) complete$(O) hilite$(O) main$(O)

atto$(E) : $(OBJ)
	$(LD) $(LDFLAGS) -o atto$(E) $(OBJ) $(LIBS)

command$(O): command.c header.h
	$(CC) $(CFLAGS) -c command.c

complete$(O): complete.c header.h
	$(CC) $(CFLAGS) -c complete.c

display$(O): display.c header.h
	$(CC) $(CFLAGS) -c display.c

gap$(O): gap.c header.h
	$(CC) $(CFLAGS) -c gap.c

key$(O): key.c header.h
	$(CC) $(CFLAGS) -c key.c

search$(O): search.c header.h
	$(CC) $(CFLAGS) -c search.c

replace$(O): replace.c header.h
	$(CC) $(CFLAGS) -c replace.c

window$(O): window.c header.h
	$(CC) $(CFLAGS) -c window.c

buffer$(O): buffer.c header.h
	$(CC) $(CFLAGS) -c buffer.c

hilite$(O): hilite.c header.h
	$(CC) $(CFLAGS) -c hilite.c

main$(O): main.c header.h
	$(CC) $(CFLAGS) -c main.c

clean:
	-$(RM) -f $(OBJ) atto$(E)

install: atto$(E)
	$(MKDIR) -p $(DESTDIR)$(BINDIR)
	$(CP) atto$(E) $(DESTDIR)$(BINDIR)

uninstall:
	-$(RM)	$(DESTDIR)$(BINDIR)/atto$(E)
	@echo not removing $(DESTDIR)$(BINDIR)

install-doc:
	$(MKDIR) -p $(DESTDIR)$(DOCDIR)
	$(CP) README.md $(DESTDIR)$(DOCDIR)/

uninstall-doc:
	$(RM) -rf $(DESTDIR)$(DOCDIR)
