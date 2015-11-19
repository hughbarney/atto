/*
 * key.h
 *
 * AttoEmacs, Hugh Barney, November 2015, A single buffer, single screen Emacs
 * Derived from: Anthony's Editor January 93, (Public Domain 1991, 1993 by Anthony Howe)
 *
 */

#ifndef __key_h__
#define __key_h__       1

#include <stdio.h>
#include <sys/types.h>

#undef _
#define _(x)    x
#define K_BUFFER_LENGTH         256
#define ISFUNCKEY(x)            ((x) < 0)

/* command key constants. */
#define K_ERROR         (-1)

/* Edit functions. */
#define K_DELETE_LEFT   (-103)
#define K_DELETE_RIGHT  (-104)
#define K_BLOCK         (-105)
#define K_CUT           (-106)
#define K_PASTE         (-107)
#define K_UNDO          (-108)

/* Cursor motion. */
#define K_CURSOR_UP     (-109)
#define K_CURSOR_DOWN   (-110)
#define K_CURSOR_LEFT   (-111)
#define K_CURSOR_RIGHT  (-112)
#define K_PAGE_UP       (-113)
#define K_PAGE_DOWN     (-114)
#define K_WORD_LEFT     (-115)
#define K_WORD_RIGHT    (-116)
#define K_LINE_LEFT     (-117)
#define K_LINE_RIGHT    (-118)
#define K_FILE_TOP      (-119)
#define K_FILE_BOTTOM   (-120)

/* Support functions. */
#define K_REDRAW        (-122)
#define K_SHOW_VERSION  (-123)
#define K_MACRO_DEFINE  (-128)
#define K_LITERAL       (-129)
#define K_QUIT          (-130)
#define K_QUIT_ASK      (-131)
#define K_FILE_INSERT   (-132)
#define K_FILE_READ     (-133)
#define K_FILE_WRITE    (-134)
#define K_STTY_ERASE    (-135)
#define K_STTY_KILL     (-136)
#define K_SAVE_BUFFER  (-137)

typedef struct keymap_t {
	short code;             /* Function code. */
	char *key_bind;
	char *lhs;              /* Left hand side invokes function or macro. */
	char *rhs;              /* Right hand side macro expansion. */

} keymap_t;

extern int getliteral _((void));
extern int getkey _((keymap_t *));
extern int getinput _((char *, int, int));
extern int ismacro _((void));

#endif /* __key_h__ */
