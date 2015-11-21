/*
 * data.c              
 *
 * AttoEmacs, Hugh Barney, November 2015, A single buffer, single screen Emacs
 * Derived from: Anthony's Editor January 93, (Public Domain 1991, 1993 by Anthony Howe)
 *
 */

#include "header.h"
#include "key.h"

int done;
int modified;

point_t point;
point_t page;
point_t epage;
point_t marker = NOMARK;

int row, col;

char_t *buf;
char_t *ebuf;
char_t *gap;
char_t *egap;

point_t nscrap;
char_t *scrap;

int input;
int msgflag;
char msgline[BUFSIZ];
char filename[BUFSIZ];
char temp[BUFSIZ];
char *prog_name;

/* key, func */
command_table_t cmd_table[] = {
	{ K_CURSOR_LEFT, left },
	{ K_CURSOR_RIGHT, right },
	{ K_CURSOR_DOWN, down },
	{ K_CURSOR_UP, up },
	{ K_WORD_LEFT, wleft },
	{ K_WORD_RIGHT, wright },
	{ K_PAGE_UP, pgup },
	{ K_PAGE_DOWN, pgdown },
	{ K_LINE_LEFT, lnbegin },
	{ K_LINE_RIGHT, lnend },
	{ K_FILE_TOP, top },
	{ K_FILE_BOTTOM, bottom },
	{ K_DELETE_LEFT, backsp },
	{ K_DELETE_RIGHT, delete },
	{ K_BLOCK, iblock },
	{ K_CUT, cut },
	{ K_COPY, copy },
	{ K_PASTE, paste },
	{ K_UNDO, undo },
	{ K_FILE_INSERT, insertfile },
	{ K_FILE_READ, readfile },
	{ K_FILE_WRITE, writefile },
	{ K_SAVE_BUFFER, savebuffer },
	{ K_REDRAW, redraw },
	{ K_QUIT, quit },
	{ K_QUIT_ASK, quit_ask },
	{ K_SHOW_VERSION, version },
	{ K_MACRO_DEFINE, macro_noop },
	{ 0, NULL }
};

keymap_t *key_map;

msg_t f_ok = "%s: Terminated successfully.\n";
msg_t f_error = "%s: Unspecified error.\n";
msg_t f_usage = "usage: %s [-f <config>] [file]\n";
msg_t f_initscr = "%s: Failed to initialize the screen.\n";
msg_t f_alloc = "%s: Failed to allocate required memory.\n";
msg_t m_ok = "Ok.";
msg_t m_version = VERSION;
msg_t m_alloc = "No more memory available.";
msg_t m_toobig = "File \"%s\" is too big to load.";
msg_t m_scrap = "Scrap is empty.  Nothing to paste.";
msg_t m_stat = "Failed to find file \"%s\".";
msg_t m_open = "Failed to open file \"%s\".";
msg_t m_close = "Failed to close file \"%s\".";
msg_t m_write = "Failed to write file \"%s\".";
msg_t m_badname = "Not a portable POSIX file name.";
msg_t m_file = "File \"%s\" %ld bytes.";
msg_t m_saved = "File \"%s\" %ld bytes saved.";
msg_t m_loaded = "File \"%s\" %ld bytes read.";
msg_t str_notsaved = "Discard changes (y/n) ?";
msg_t str_querysave = "Save changes (y/n) ?";
msg_t str_read = "Find file: ";
msg_t str_insert_file ="Insert file: ";
msg_t str_write = "Write file: ";
msg_t str_yes = " y\b";
msg_t str_no = " n\b";
msg_t str_mark = "Mark set";
