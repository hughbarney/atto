/*
 * main.c              
 *
 * Anthony's Editor January 93
 *
 * Public Domain 1991, 1993 by Anthony Howe.  No warranty.
 */

#include <ctype.h>
#include <errno.h>
#include <string.h>
#include "header.h"
#include <stdarg.h>

void debug(char *, ...);

keymap_t keymap2[] = {
 {K_CURSOR_UP         , "C-p                      ", "\x1B\x5B\x41","NULL"},
 {K_CURSOR_DOWN       , "C-n                      ", "\x1B\x5B\x42","NULL"},
 {K_CURSOR_LEFT       , "C-b                      ", "\x1B\x5B\x44","NULL"},
 {K_CURSOR_RIGHT      , "C-f                      ", "\x1B\x5B\x43","NULL"},
 {K_LINE_LEFT         , "C-a beginning-of-line    ", "\x01","NULL"},
 {K_LINE_LEFT         , "C-a beginning-of-line    ", "\x1B\x4F\x48","NULL"}, /* home key */
 {K_CURSOR_LEFT       , "C-b                      ", "\x02","NULL"},
 {K_DELETE_RIGHT      , "C-d forward-delete-char  ", "\x04","NULL"},
 {K_DELETE_RIGHT      , "DEL forward-delete-char  ", "\x1B\x5B\x33\x7E","NULL"}, /* Del key */
 {K_DELETE_LEFT       , "backspace delete-left    ", "\x7f","NULL"},
 {K_LINE_RIGHT        , "C-e end-of-line          ", "\x05","NULL"},
 {K_LINE_RIGHT        , "end end-of-line          ", "\x1B\x4F\x46","NULL"}, /* end key */
 {K_CURSOR_RIGHT      , "C-f                      ", "\x06","NULL"},
 {K_DELETE_LEFT       , "C-h backspace            ", "\x08","NULL"},
 {K_REDRAW            , "C-l                      ", "\x0C","NULL"},
 {K_CURSOR_DOWN       , "C-n                      ", "\x0E","NULL"},
 {K_CURSOR_UP         , "C-p                      ", "\x10","NULL"},
 {K_UNDO              , "C-u                      ", "\x15","NULL"},
 {K_PAGE_DOWN         , "C-v                      ", "\x16","NULL"},
 {K_PAGE_DOWN         , "PgDn                     ", "\x1B\x5B\x36\x7E","NULL"}, /* PgDn key */
 {K_FILE_TOP          , "esc < beg-of-buf         ", "\x1B\x3C","NULL"},
 {K_FILE_BOTTOM       , "esc > end-of-buf         ", "\x1B\x3E","NULL"},
 {K_PAGE_UP           , "esc v                    ", "\x1B\x76","NULL"},
 {K_PAGE_UP           , "PgUp                     ", "\x1B\x5B\x35\x7E","NULL"}, /* PgUp key */
 {K_WORD_LEFT         , "esc b back-word          ", "\x1B\x62","NULL"},
 {K_WORD_RIGHT        , "esc f forward-word       ", "\x1B\x66","NULL"},
 {K_BLOCK             , "C-space set-amrk         ", "","NULL"},
 {K_CUT               , "C-w                      ", "\x17","NULL"},    /* c-w not working on chrome book */
 {K_CUT               , "C-i                      ", "\x09","NULL"},    /* c-i use for now */
 {K_PASTE             , "C-y                      ", "\x19","NULL"},
 {K_MACRO_DEFINE      , "C-k                      ", "\x0B","\x01\x00\x05\x09"},
 {K_FILE_READ         , "C-x C-f find-file        ", "\x18\x06","NULL"},
 {K_FILE_WRITE        , "C-x C-d write-file       ", "\x18\x04","NULL"},
 {K_QUIT              , "C-x C-c                  ", "\x18\x03","NULL"},
 {K_SHOW_VERSION      , "esc esc show-version     ", "\x1B\x1B","NULL"},
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

		key_map = keymap2;
		modeless = TRUE;
        noecho();
        lineinput(FALSE);
        idlok(stdscr, TRUE);

        if (1 < argc) {
                (void) load(argv[1]);
                /* Save filename irregardless of load() success. */
                strcpy(filename, argv[1]);
                modified = FALSE;
        }
        if (!growgap(CHUNK))
                fatal(f_alloc);

        top();
        i = msgflag;
        msgflag = i;
        while (!done) {
                display();
                i = 0;
                input = getkey(key_map);
                while (table[i].key != 0 && input != table[i].key)
                        ++i;
                if (table[i].func != NULL)
					(*table[i].func)();
                else
					insert();
        }
        if (scrap != NULL)
                free(scrap);
        move(LINES-1, 0);
        refresh();
        endwin();
        putchar('\n');
        return (EXIT_OK);
}

#ifdef TERMIOS
#include <termios.h>

/*
 *      Set the desired input mode.
 *
 *      FALSE enables immediate character processing (disable line processing
 *      and signals for INTR, QUIT, and SUSP).  TRUE enables line processing
 *      and signals (disables immediate character processing).  In either
 *      case flow control (XON/XOFF) is still active.  
 *
 *      If the termios function calls fail, then fall back on using
 *      CURSES' cbreak()/nocbreak() functions; however signals will be
 *      still be in effect.
 */
void lineinput(int bf)
{
        int error;
        struct termios term;
        error = tcgetattr(fileno(stdin), &term) < 0;
        if (!error) {
                if (bf)
                        term.c_lflag |= ISIG | ICANON;
                else
                        term.c_lflag &= ~(ISIG | ICANON);
                error = tcsetattr(fileno(stdin), TCSANOW, &term) < 0;
        }
        /* Fall back on CURSES functions that do almost what we need if
         * either tcgetattr() or tcsetattr() fail.
         */
        if (error) {
                if (bf)
                        nocbreak();
                else
                        cbreak();
        }
}

#endif /* TERMIOS */

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
