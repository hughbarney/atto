/*
 * data.c              
 *
 * Anthony's Editor January 93
 *
 * Public Domain 1991, 1993 by Anthony Howe.  No warranty.
 */

#include "header.h"
#include "key.h"

int done;
int modified;
int modeless;

point_t point;
point_t page;
point_t epage;
point_t marker = NOMARK;

int row, col;
int textline = HELPLINE;

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

keytable_t table[] = {
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
        { K_INSERT_ENTER, insert_mode },
        { K_BLOCK, block },
        { K_CUT, cut },
        { K_PASTE, paste },
        { K_UNDO, undo },
        { K_FILE_READ, readfile },
        { K_FILE_WRITE, writefile },
        { K_REDRAW, redraw },
        { K_HELP, help },
        { K_QUIT, quit },
        { K_QUIT_ASK, quit_ask },
        { K_SHOW_VERSION, version },
        { K_MACRO, macro },
        { 0, NULL }

};

keymap_t key_mode[] = {
        { K_INSERT_EXIT, NULL },
        { K_STTY_ERASE, NULL },
        { K_LITERAL, NULL },
        { K_ERROR, NULL }

};

keymap_t *key_map;

msg_t f_ok = "%s: Terminated successfully.\n";
msg_t f_error = "%s: Unspecified error.\n";
msg_t f_usage = "usage: %s [-f <config>] [file]\n";
msg_t f_initscr = "%s: Failed to initialize the screen.\n";
msg_t f_config = "%s: Problem with configuration file.\n";
msg_t f_alloc = "%s: Failed to allocate required memory.\n";

msg_t m_ok = "Ok.";
msg_t m_version = VERSION;
msg_t m_error = "An unknown error occured.";
msg_t m_alloc = "No more memory available.";
msg_t m_toobig = "File \"%s\" is too big to load.";
msg_t m_scrap = "Scrap is empty.  Nothing to paste.";
msg_t m_stat = "Failed to find file \"%s\".";
msg_t m_open = "Failed to open file \"%s\".";
msg_t m_close = "Failed to close file \"%s\".";
msg_t m_read = "Failed to read file \"%s\".";
msg_t m_write = "Failed to write file \"%s\".";
msg_t m_badname = "Not a portable POSIX file name.";
msg_t m_file = "File \"%s\" %ld bytes.";
msg_t m_saved = "File \"%s\" %ld bytes saved.";
msg_t m_loaded = "File \"%s\" %ld bytes read.";
msg_t m_modified = "File \"%s\" modified.";
msg_t m_lhsbad = "Left-hand-side of macro is incorrectly specified.";
msg_t m_rhsbad = "Right-hand-side of macro is incorrectly specified.";
msg_t m_nomacro = "No such macro defined.";
msg_t m_slots = "No more macro space.";

msg_t str_macro = "Macro :";
msg_t str_notsaved = "File not saved.  Quit (y/n) ?";
msg_t str_press = "[ Press a key to continue. ]";
msg_t str_read = "Read file :";
msg_t str_write = "Write file :";
msg_t str_more = " more ";
msg_t str_yes = " y\b";
msg_t str_no = " n\b";
msg_t str_quit = " q\b";

/* end */
