/*
 * header.h            
 *
 * Anthony's Editor January 93
 *
 * Public Domain 1991, 1993 by Anthony Howe.  No warranty.
 */

#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <curses.h>
#include "key.h"

#undef _
#define _(x)    x

#define VERSION															\
	"Atto March 2014. Public Domain 2014, by Hugh Barney.  No warranty."

#ifndef CHUNK
#define CHUNK           8096L
#endif /* CHUNK */

/* Exit status. */
#define EXIT_OK         0               /* Success */
#define EXIT_ERROR      1               /* Unknown error. */
#define EXIT_USAGE      2               /* Usage */
#define EXIT_FAIL       3               /* Known failure. */

/* Screen partitioning. */
#define MSGLINE          (LINES-1)
#define MODELINE         (LINES-2)
#define FIRST_LINE       0
#define MAXLINE          (LINES-2)


#define NOMARK          -1

typedef char *msg_t;
typedef unsigned char char_t;
typedef long point_t;

typedef struct command_table_t {
	short key;
	void (*func) _((void));

} command_table_t;

/*
 * Some compilers define size_t as a unsigned 16 bit number while
 * point_t and off_t might be defined as a signed 32 bit number.  
 * malloc(), realloc(), fread(), and fwrite() take size_t parameters,
 * which means there will be some size limits because size_t is too
 * small of a type.
 */
#define MAX_SIZE_T      ((unsigned long) (size_t) ~0)

extern int done;                /* Quit flag. */
extern int modified;            /* Text buffer modified flag. */
extern int msgflag;             /* True if msgline should be displayed. */

extern int row;                 /* Cursor screen row */
extern int col;                 /* Cursor screen column. */

extern point_t point;           /* Cursor offset in text buffer. */
extern point_t page;            /* Top of screen page. */
extern point_t epage;           /* End of screen page +1 */
extern point_t marker;          /* Block anchor point. */

extern char_t *buf;             /* Base of allocated text buffer. */
extern char_t *ebuf;            /* End of text buffer +1 */
extern char_t *gap;             /* Start of gap. */
extern char_t *egap;            /* End of gap +1 */

extern point_t nscrap;          /* Length of scrap buffer. */
extern char_t *scrap;           /* Allocated scrap buffer. */

extern int input;               /* Current input character. */
extern char msgline[];          /* Message line input/output buffer. */
extern char filename[];         /* Current filename for text buffer. */
extern char temp[];             /* Temporary buffer. */
extern char *prog_name;         /* Name used to invoke editor. */

extern command_table_t cmd_table[];      /* Command jump table. */
extern keymap_t *key_map;       /* Command key mappings. */

/* fatal() messages. */
extern msg_t f_ok;              /* EXIT_OK */
extern msg_t f_error;           /* EXIT_ERROR */
extern msg_t f_usage;           /* EXIT_USAGE */
extern msg_t f_initscr;         /* EXIT_FAILURE ... */
extern msg_t f_alloc;

/* Messages. */
extern msg_t m_ok;
extern msg_t m_version;
extern msg_t m_alloc;
extern msg_t m_toobig;
extern msg_t m_scrap;
extern msg_t m_stat;
extern msg_t m_open;
extern msg_t m_close;
extern msg_t m_read;
extern msg_t m_write;
extern msg_t m_badname;
extern msg_t m_saved;
extern msg_t m_loaded;

/* Prompts */
extern msg_t str_notsaved;
extern msg_t str_querysave;
extern msg_t str_read;
extern msg_t str_insert_file;
extern msg_t str_write;
extern msg_t str_yes;
extern msg_t str_no;

extern void fatal _((msg_t));
extern void msg _((msg_t, ...));
extern void display _((void));
extern point_t lnstart _((point_t));
extern point_t lncolumn _((point_t, int));
extern point_t segstart _((point_t, point_t));
extern point_t segnext _((point_t, point_t));
extern point_t upup _((point_t));
extern point_t dndn _((point_t));

extern int growgap _((point_t));
extern point_t movegap _((point_t));
extern point_t pos _((char_t *));
extern char_t *ptr _((point_t));
extern int posix_file _((char *));
extern int save _((char *));
extern int load_file _((char *));
extern int insert_file (char *, int);
extern void undoset _((void));
extern void undo _((void));
extern void backsp _((void));
extern void block _((void));
extern void bottom _((void));
extern void cut _((void));
extern void delete _((void));
extern void down _((void));
extern void insert _((void));
extern void left _((void));
extern void lnbegin _((void));
extern void lnend _((void));
extern void paste _((void));
extern void pgdown _((void));
extern void pgup _((void));
extern void quit _((void));
extern void quit_ask _((void));
extern void redraw _((void));
extern void readfile _((void));
extern void insertfile _((void));
extern void right _((void));
extern void top _((void));
extern void up _((void));
extern void version _((void));
extern void wleft _((void));
extern void wright _((void));
extern void writefile _((void));
extern void savebuffer _((void));

extern void debug(char *, ...);
extern void debug_stats(char *);
extern void modeline(void);
extern void macro_noop(void);
