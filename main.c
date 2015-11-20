/*
 * main.c              
 *
 * AttoEmacs, Hugh Barney, November 2015, A single buffer, single screen Emacs
 * Derived from: Anthony's Editor January 93, (Public Domain 1991, 1993 by Anthony Howe)
 */

#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "header.h"
#include <stdarg.h>

/* code, desc, lhs, rhs */
keymap_t keymap[] = {
	{K_CURSOR_UP         , "up previous-line         ", "\x1B\x5B\x41","NULL"},
	{K_CURSOR_DOWN       , "down next-line           ", "\x1B\x5B\x42","NULL"},
	{K_CURSOR_LEFT       , "left backward-character  ", "\x1B\x5B\x44","NULL"},
	{K_CURSOR_RIGHT      , "right forward-character  ", "\x1B\x5B\x43","NULL"},

	{K_LINE_LEFT         , "C-a beginning-of-line    ", "\x01","NULL"},
	{K_LINE_RIGHT        , "C-e end-of-line          ", "\x05","NULL"},
	{K_LINE_LEFT         , "home beginning-of-line   ", "\x1B\x4F\x48","NULL"}, /* home key */
	{K_LINE_RIGHT        , "end end-of-line          ", "\x1B\x4F\x46","NULL"}, /* end key */

	{K_CURSOR_LEFT       , "C-b                      ", "\x02","NULL"},
	{K_CURSOR_RIGHT      , "C-f                      ", "\x06","NULL"},
	{K_CURSOR_DOWN       , "C-n                      ", "\x0E","NULL"},
	{K_CURSOR_UP         , "C-p                      ", "\x10","NULL"},
 
	{K_DELETE_RIGHT      , "C-d forward-delete-char  ", "\x04","NULL"},
	{K_DELETE_RIGHT      , "DEL forward-delete-char  ", "\x1B\x5B\x33\x7E","NULL"}, /* Del key */
	{K_DELETE_LEFT       , "backspace delete-left    ", "\x7f","NULL"},
	{K_DELETE_LEFT       , "C-h backspace            ", "\x08","NULL"},
	{K_REDRAW            , "C-l                      ", "\x0C","NULL"},
	{K_UNDO              , "C-u                      ", "\x15","NULL"},
	{K_PAGE_UP           , "esc v                    ", "\x1B\x76","NULL"},
	{K_PAGE_DOWN         , "C-v                      ", "\x16","NULL"},
	{K_PAGE_UP           , "PgUp                     ", "\x1B\x5B\x35\x7E","NULL"}, /* PgUp key */
	{K_PAGE_DOWN         , "PgDn                     ", "\x1B\x5B\x36\x7E","NULL"}, /* PgDn key */

	{K_FILE_TOP          , "esc < beg-of-buf         ", "\x1B\x3C","NULL"},
	{K_FILE_BOTTOM       , "esc > end-of-buf         ", "\x1B\x3E","NULL"},
	{K_FILE_TOP          , "esc home, end-of-buf     ", "\x1B\x1B\x4F\x48","NULL"},
	{K_FILE_BOTTOM       , "esc end, beg-of-buf      ", "\x1B\x1B\x4F\x46","NULL"},
	{K_FILE_TOP          , "esc up, beg-of-buf       ", "\x1B\x1B\x5B\x41","NULL"},
	{K_FILE_BOTTOM       , "esc down, end-of-buf     ", "\x1B\x1B\x5B\x42","NULL"},
 
	{K_WORD_LEFT         , "esc b back-word          ", "\x1B\x62","NULL"},
	{K_WORD_RIGHT        , "esc f forward-word       ", "\x1B\x66","NULL"},
	{K_BLOCK             , "C-space set-mark         ", "\x00","NULL"},    /* ctrl-space, note is NULL, cant use 0x00 is macros */
	{K_BLOCK             , "C-/ set-mark             ", "\x0F","NULL"},    /* alternate, to enable use in macros */
	{K_CUT               , "C-w kill-region          ", "\x17","NULL"},    /* c-w not working on chrome book */
	{K_PASTE             , "C-y yank                 ", "\x19","NULL"},
	{K_COPY              , "esc w copy-region        ", "\x1B\x77","NULL"},

	{K_MACRO_DEFINE      , "C-k kill-to-eol          ", "\x0B","\x0F\x05\x17"}, /*C-K => C-/, C-E, C-W */
	{K_FILE_INSERT       , "C-x i insert-file        ", "\x18\x69","NULL"},
	{K_FILE_READ         , "C-x C-f find-file        ", "\x18\x06","NULL"},
	{K_SAVE_BUFFER       , "C-x C-s save-buffer      ", "\x18\x13","NULL"},  
	{K_FILE_WRITE        , "C-x C-w write-file       ", "\x18\x17","NULL"},  /* write and prompt for name */
	{K_QUIT_ASK          , "C-x C-c exit             ", "\x18\x03","NULL"},
// {K_SHOW_VERSION      , "esc esc show-version     ", "\x1B\x1B","NULL"},
	{K_ERROR             , "K_ERROR                  ", NULL, NULL }
};

int main(int argc, char **argv)
{
	int i;
		
	/* Find basename. */
	prog_name = *argv;
	i = strlen(prog_name);
	while (0 <= i && prog_name[i] != '\\' && prog_name[i] != '/')
		--i;
	prog_name += i+1;

	if (initscr() == NULL)
		fatal(f_initscr);

	raw();
	noecho();
	idlok(stdscr, TRUE);
	filename[0] = '\0';
		
	if (1 < argc) {
		(void) insert_file(argv[1], FALSE);
		/* Save filename irregardless of load() success. */
		strcpy(filename, argv[1]);
		modified = FALSE;
	}

	if (!growgap(CHUNK))
		fatal(f_alloc);

	key_map = keymap;
	top();

	while (!done) {
		display();
		i = 0;
		input = getkey(key_map);
		while (cmd_table[i].key != 0 && input != cmd_table[i].key)
			++i;
		if (cmd_table[i].func != NULL) {
			(*cmd_table[i].func)();
		} else {
			insert();
		}
		//debug_stats("main loop:");
	}
	if (scrap != NULL)
		free(scrap);

	move(LINES-1, 0);
	refresh();
	noraw();
	endwin();

	return (EXIT_OK);
}

void fatal(msg_t m)
{
	if (curscr != NULL) {
		move(LINES-1, 0);
		refresh();
		endwin();
		putchar('\n');
	}
	fprintf(stderr, m, prog_name);
	if (m == f_ok)
		exit(EXIT_OK);
	if (m == f_error)
		exit(EXIT_ERROR);
	if (m == f_usage)
		exit(EXIT_USAGE);
	exit(EXIT_FAIL);
}

void msg(msg_t m, ...)
{
	va_list args;
	va_start(args, m);
	(void) vsprintf(msgline, m, args);
	va_end(args);
	msgflag = TRUE;
}

void debug(char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start (args, format);

    static FILE *debug_fp = NULL;

    if (debug_fp == NULL) {
        debug_fp = fopen("debug.out","w");
    }

    vsprintf (buffer, format, args);
    va_end(args);

    fprintf(debug_fp,"%s", buffer);
    fflush(debug_fp);
}

void debug_stats(char *s) {
	debug("%s bsz=%d gap=%d egap=%d\n", s, ebuf - buf, gap - buf, egap - buf);
}
