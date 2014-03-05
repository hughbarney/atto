/*
 * key.h
 *
 * Anthony's Editor January 93
 *
 * Public Domain 1991, 1993 by Anthony Howe.  No warranty.
 */

#ifndef __key_h__
#define __key_h__       1

#include <stdio.h>
#include <sys/types.h>

#undef _
#ifdef __STDC__
#define _(x)    x
#else
#define _(x)    ()
#endif

#define K_BUFFER_LENGTH         256
#define ISFUNCKEY(x)            ((x) < 0)

/*
 * Command key constants.
 */
#define K_ERROR         (-1)
#define K_DISABLED      (-2)

/* Edit functions. */
#define K_INSERT_ENTER  (-101)
#define K_INSERT_EXIT   (-102)
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
#define K_ITSELF        (-121)
#define K_REDRAW        (-122)
#define K_SHOW_VERSION  (-123)
#define K_HELP          (-124)
#define K_HELP_OFF      (-125)
#define K_HELP_TEXT     (-126)
#define K_MACRO         (-127)
#define K_MACRO_DEFINE  (-128)
#define K_LITERAL       (-129)
#define K_QUIT          (-130)
#define K_QUIT_ASK      (-131)
#define K_FILE_READ     (-132)
#define K_FILE_WRITE    (-133)
#define K_STTY_ERASE    (-134)
#define K_STTY_KILL     (-135)

#define K_MAX_CODES     36

/*
 * Function error codes.
 */
#define INITKEY_OK      0
#define INITKEY_OPEN    1
#define INITKEY_ALLOC   2
#define INITKEY_ERROR   3

/*
 * ASCII Control Codes
 */
#undef CTRL
#define CTRL(x)         ((x) & 0x1f)

typedef struct keymap_t {
	short code;             /* Function code. */
	char *bind_desc;
	char *lhs;              /* Left hand side invokes function or macro. */
	char *rhs;              /* Right hand side macro expansion. */

} keymap_t;

typedef struct keyinit_t {
	short code;
	char *key_sym;
	char *bind_desc;
	char *lhs;
	int (*fn) _((FILE *, char *, keymap_t *));

} keyinit_t;

extern int initkey _((char *, keymap_t **));
extern void finikey _((keymap_t *));
extern keymap_t *findkey _((keymap_t *, char *));
extern keyinit_t *findikey _((keyinit_t *, char *));
extern size_t encodekey _((char *, char *));
extern int getliteral _((void));
extern int getkey _((keymap_t *));
extern int getinput _((char *, int, int));
extern int ismacro _((void));
extern keyinit_t *find_keyword(int);


#endif /* __key_h__ */
