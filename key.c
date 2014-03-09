/*
 * key.c                
 *
 * Anthony's Editor January 93
 *
 * Public Domain 1991, 1993 by Anthony Howe.  No warranty.
 */

#include <ctype.h>
#include <string.h>
#include "header.h"
#include "key.h"

/* Variable length structure. */
typedef struct input_t {
        struct input_t *next;
        char *ptr;
        char buf[1];

} input_t;

static input_t *istack = NULL;
static int ipush _((char *));
static int ipop _((void));

int getkey(keymap_t *keys)
{
        keymap_t *k;
        int submatch;
        static char buffer[K_BUFFER_LENGTH];
        static char *record = buffer;

        /* If recorded bytes remain, return next recorded byte. */
        if (*record != '\0')
                return (*(unsigned char *)record++);
        /* Reset record buffer. */
        record = buffer;
        do {
                if (K_BUFFER_LENGTH < record - buffer) {
                        record = buffer;
                        buffer[0] = '\0';
                        return (K_ERROR);
                }
                /* Read and record one byte. */
                *record++ = getliteral();
                *record = '\0';

                /* If recorded bytes match any multi-byte sequence... */
                for (k = keys, submatch = 0; k->code != K_ERROR; ++k) {
                        char *p, *q;
                        if (k->lhs == NULL)
                                continue;
                        for (p = buffer, q = k->lhs; *p == *q; ++p, ++q) {
                                if (*q == '\0') {
                                        if (k->code == K_LITERAL)
                                                return (getliteral());
                                        if (k->code != K_MACRO_DEFINE) {
                                                /* Return extended key code. */
                                                return (k->code);
                                        }
                                        if (k->rhs != NULL)
                                                (void) ipush(k->rhs);
                                        break;
                                }
                        }
                        if (*p == '\0' && *q != '\0') {
                                /* Recorded bytes match anchored substring. */
                                submatch = 1;
                        }
                }
                /* If recorded bytes matched an anchored substring, loop. */
        } while (submatch);
        /* Return first recorded byte. */
        record = buffer;
        return (*(unsigned char *)record++);
}

int getliteral()
{
        int ch;

        ch = ipop();
        if (ch == EOF)
                return ((unsigned) getch());
        return (ch);
}

/*
 * Return true if a new input string was pushed onto the stack,
 * else false if there was no more memory for the stack.
 */
static int ipush(buf)
char *buf;
{
        input_t *new;

        new = (input_t *) malloc(sizeof (input_t) + strlen (buf));
        if (new == NULL)
                return (FALSE);
        (void) strcpy(new->buf, buf);
        new->ptr = new->buf;
        new->next = istack;
        istack = new;
        return (TRUE);
}

/*
 * Pop and return a character off the input stack.  Return EOF if
 * the stack is empty.  If the end of an input string is reached,
 * then free the node.  This will allow clean tail recursion that
 * won't blow the stack.  
 */
static int ipop()
{
        int ch;
        input_t *node;

        if (istack == NULL)
                return (EOF);
        ch = (unsigned) *istack->ptr++;
        if (*istack->ptr == '\0') {
                node = istack;
                istack = istack->next;
                free(node);
        }
        return (ch);

}

int ismacro()
{
        return (istack != NULL);
}

/*
 * Field input.
 */
typedef struct key_entry_t {
        int code;
        int (*func) _((void));

} key_entry_t;

static int fld_done _((void));
static int fld_erase _((void));
static int fld_kill _((void));
static int fld_left _((void));
static int fld_insert _((void));

#define ERASE_KEY       0
#define KILL_KEY        1

static key_entry_t ktable[] = {
        { K_STTY_ERASE, fld_erase },
        { K_STTY_KILL, fld_kill },
        { '\r', fld_done },
        { '\n', fld_done },
        { '\b', fld_erase },
        { -1, fld_insert }

};

static int fld_row;
static int fld_col;
static int fld_key;
static int fld_echo;
static int fld_index;
static int fld_length;
static char *fld_buffer;

#ifndef getmaxyx
#define getmaxyx(w,r,c)         (r=LINES,c=COLS)
#endif

int getinput(buf, len, echoing)
char *buf;
int len, echoing;
{
        int first;
        key_entry_t *k;
        fld_buffer = buf;
        fld_index = (int) strlen(fld_buffer);
        fld_length = len < 0 ? COLS : len;
        if (--fld_length < 1)
                return (FALSE);
        ktable[ERASE_KEY].code = erasechar();
        ktable[KILL_KEY].code = killchar();    
        fld_echo = echoing;
        getyx(stdscr, fld_row, fld_col);
        addstr(fld_buffer);
        move(fld_row, fld_col);
        for (first = TRUE;; first = FALSE) {
                refresh();
                fld_key = getliteral();
                for (k = ktable; k->code != -1 && k->code != fld_key; ++k)
                        ;
                if (first && k->func == fld_insert)
                        fld_kill();
                if (k->func != NULL && !(*k->func)()) {
                        fld_buffer[fld_index] = '\0';
                        break;
                }
        }
        return (TRUE);

}

static int fld_done() 
{
        return (FALSE);
}

static int fld_left()
{
        int row, col, max_row, max_col;
        getyx(stdscr, row, col);
        getmaxyx(stdscr, max_row, max_col);
        if (0 < fld_index) {
                --fld_index;
                /* Assume that if 0 < fld_index then fld_row <= row
                 * and fld_col < col.  So when fld_index == 0, then
                 * fld_row == row and fld_col == col.
                 */
                if (0 < col) {
                        --col;
                } else if (0 < row) {
                        /* Handle reverse line wrap. */
                        --row;
                        col = max_col-1;
                }
                move(row, col);
        }
        return (TRUE);

}

static int fld_erase()
{
        int row, col;
        if (0 < fld_index) {
                fld_left();
                getyx(stdscr, row, col);
                addch(' ');
                move(row, col);
                fld_buffer[fld_index] = '\0';
        }
        return (TRUE);

}

static int fld_kill()
{
        move(fld_row, fld_col);
        while (0 < fld_index--)
                addch(' ');
        move(fld_row, fld_col);
        fld_buffer[0] = '\0';
        fld_index = 0;
        return (TRUE);

}

static int fld_insert()
{
        if (fld_index < fld_length) {
                if (!ISFUNCKEY(fld_key)) {
                        fld_buffer[fld_index++] = fld_key;
                        if (fld_echo)
                                addch(fld_key);
                }
        }
        return (fld_index < fld_length);
}
