/*
 * header.h            
 *
 * AttoEmacs, Hugh Barney, November 2015
 * Derived from: Anthony's Editor January 93, (Public Domain 1991, 1993 by Anthony Howe)
 *
 */

#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <curses.h>
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>

#undef _
#define _(x)    x

#define VERSION	 "Atto 1.4.1, Public Domain, December 2015, by Hugh Barney,  No warranty."

/* Exit status. */
#define EXIT_OK         0               /* Success */
#define EXIT_ERROR      1               /* Unknown error. */
#define EXIT_USAGE      2               /* Usage */
#define EXIT_FAIL       3               /* Known failure. */

#define MSGLINE         (LINES-1)
#define NOMARK          -1
#define CHUNK           8096L
#define K_BUFFER_LENGTH 256
#define STRBUF_L        256
#define STRBUF_M        64
#define STRBUF_S        16
#define MIN_GAP_EXPAND  512

typedef char *msg_t;
typedef unsigned char char_t;
typedef long point_t;

typedef struct keymap_t {
	char *key_bind;
	char *lhs;              /* Left hand side invokes function or macro. */
	void (*func) _((void));
} keymap_t;

typedef struct undo_t {
	point_t u_point;
	point_t u_gap;
	point_t u_egap;

} undo_t;

typedef struct buffer_t
{
	struct buffer_t *b_next;  /* Link to next buffer_t */
	point_t b_mark;	     	  /* the mark */
	point_t b_point;          /* the point */
	point_t b_page;           /* start of page */
	point_t b_epage;          /* end of page */
	int b_modified;           /* modified ? */
	int b_cnt;                /* count of windows referencing this buffer */
	char_t *b_buf;            /* start of buffer */
	char_t *b_ebuf;           /* end of buffer */
	char_t *b_gap;            /* start of gap */
	char_t *b_egap;           /* end of gap */
	int b_row;                /* cursor row */
	int b_col;                /* cursor col */
	char b_fname[STRBUF_L];	  /* filename */
	char b_bname[STRBUF_S];   /* buffer name */
	undo_t b_ubuf;            /* undoset */
} buffer_t;

typedef struct window_t
{
	struct window_t *w_next;   /* Next window */
	struct buffer_t *w_bufp;   /* Buffer displayed in window */
	point_t w_point;
	point_t w_mark;
	point_t w_page;
	point_t w_epage;
	char w_top;	        /* Origin 0 top row of window */
	char w_rows;        /* no. of rows of text in window */
	int w_row;          /* cursor row */
	int w_col;          /* cursor col */
	int w_update;
	char w_name[STRBUF_S];
} window_t;

extern buffer_t *curbp;			/* current buffer */
extern buffer_t *bheadp;			/* head of list of buffers */
extern window_t *curwp;
extern window_t *wheadp;

/*
 * Some compilers define size_t as a unsigned 16 bit number while
 * point_t and off_t might be defined as a signed 32 bit number.  
 * malloc(), realloc(), fread(), and fwrite() take size_t parameters,
 * which means there will be some size limits because size_t is too
 * small of a type.
 */
#define MAX_SIZE_T      ((unsigned long) (size_t) ~0)

extern int done;                /* Quit flag. */
extern int msgflag;             /* True if msgline should be displayed. */
extern int result;

extern point_t nscrap;          /* Length of scrap buffer. */
extern char_t *scrap;           /* Allocated scrap buffer. */

extern int input;               /* Current input character. */
extern char msgline[];          /* Message line input/output buffer. */
extern char temp[];             /* Temporary buffer. */
extern char searchtext[];
extern char replace[];
extern char *prog_name;         /* Name used to invoke editor. */

extern keymap_t *key_map;       /* Command key mappings. */
extern keymap_t keymap[];
extern keymap_t *key_return;    /* Command key return */

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
extern msg_t m_newfile;
extern msg_t m_line;
extern msg_t m_lnot_found;
extern msg_t m_replace;
extern msg_t m_with;
extern msg_t m_sprompt;
extern msg_t m_qreplace;
extern msg_t m_rephelp;
extern msg_t m_goto;
extern msg_t str_mark;
extern msg_t str_pos;
extern msg_t str_endpos;

/* Prompts */
extern msg_t str_notsaved;
extern msg_t str_modified_buffers;
extern msg_t str_read;
extern msg_t str_insert_file;
extern msg_t str_write;
extern msg_t str_yes;
extern msg_t str_no;
extern msg_t str_scratch;

extern void fatal _((msg_t));
extern void msg _((msg_t, ...));
extern void display (window_t *, int);
extern void dispmsg(void);
extern void modeline(window_t *);

extern point_t lnstart (buffer_t *, point_t);
extern point_t lncolumn (buffer_t *, point_t, int);
extern point_t segstart (buffer_t *, point_t, point_t);
extern point_t segnext (buffer_t *, point_t, point_t);
extern point_t upup (buffer_t *, point_t);
extern point_t dndn (buffer_t *, point_t);

extern int getkey _((keymap_t *, keymap_t **));
extern int getinput _((char *, char *, int));
extern int growgap (buffer_t *, point_t);
extern point_t movegap (buffer_t *, point_t);
extern point_t pos (buffer_t *, char_t *);
extern char_t *ptr (buffer_t *, point_t);
extern int posix_file _((char *));
extern int save _((char *));
extern int load_file _((char *));
extern int insert_file (char *, int);
extern void undoset _((void));
extern void undo _((void));
extern void backsp _((void));
extern void block _((void));
extern void iblock _((void));
extern void bottom _((void));
extern void cut _((void));
extern void copy _((void));
extern void copy_cut _((int));
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
extern int yesno _((int));
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

extern void showpos(void);
extern void killtoeol(void);
extern void gotoline(void);
extern void search(void);
extern void query_replace(void);
extern point_t line_to_point(int);

extern point_t search_forward(char *);
extern point_t search_backwards(char *);
extern void update_search_prompt(char *, char *);
extern void display_search_result(point_t, int, char *, char *);
extern buffer_t* find_buffer (char *, int);
extern void buffer_init(buffer_t *);
extern int delete_buffer(buffer_t *);
extern void next_buffer(void);
extern int count_buffers(void);
extern int modified_buffers(void);
extern void killbuffer(void);
extern char* get_buffer_name(buffer_t *);
extern void get_line_stats(int *, int *);
extern void query_replace(void);

extern window_t *new_window();
extern void one_window(window_t *);
extern void split_window();
extern void next_window();
extern void delete_other_windows();
extern void free_other_windows();
extern void update_display();

extern void w2b(window_t *);
extern void b2w(window_t *);
extern void associate_b2w(buffer_t *, window_t *);
extern void disassociate_b(window_t *);
