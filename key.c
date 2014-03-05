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

extern void debug(char *, ...);
void debug_keyload(keymap_t *);

/* Variable length structure. */
typedef struct input_t {
        struct input_t *next;
        char *ptr;
        char buf[1];

} input_t;

static input_t *istack = NULL;
static char blank[] = " \t\r\n";

static int k_default _((FILE *, char *, keymap_t *));
static int k_erase  _((FILE *, char *, keymap_t *));
static int k_kill _((FILE *, char *, keymap_t *));
static int ipush _((char *));
static int ipop _((void));

keyinit_t keywords[] = {
	{ K_INSERT_ENTER ,"K_INSERT_ENTER" , "                        ", ".insert_enter", k_default },
	{ K_INSERT_EXIT  ,"K_INSERT_EXIT"  , "                        ", ".insert_exit", k_default },
	{ K_DELETE_LEFT  ,"K_DELETE_LEFT"  , "C-h backspace           ", ".delete_left", k_default },
	{ K_DELETE_RIGHT ,"K_DELETE_RIGHT" , "C-d forward-delete-char ", ".delete_right", k_default },
	{ K_BLOCK        ,"K_BLOCK"        , "C-space set-amrk        ", ".block", k_default },
	{ K_CUT          ,"K_CUT"          , "C-w                     ", ".cut", k_default },
	{ K_PASTE        ,"K_PASTE"        , "C-y                     ", ".paste", k_default },
	{ K_UNDO         ,"K_UNDO"         , "C-u                     ", ".undo", k_default },
	{ K_CURSOR_UP    ,"K_CURSOR_UP"    , "C-p                     ", ".cursor_up", k_default },
	{ K_CURSOR_DOWN  ,"K_CURSOR_DOWN"  , "C-n                     ", ".cursor_down", k_default },
	{ K_CURSOR_LEFT  ,"K_CURSOR_LEFT"  , "C-b                     ", ".cursor_left", k_default },
	{ K_CURSOR_RIGHT ,"K_CURSOR_RIGHT" , "C-f                     ", ".cursor_right", k_default },
	{ K_PAGE_UP      ,"K_PAGE_UP"      , "esc v                   ", ".page_up", k_default },
	{ K_PAGE_DOWN    ,"K_PAGE_DOWN"    , "C-v                     ", ".page_down", k_default },
	{ K_WORD_LEFT    ,"K_WORD_LEFT"    , "esc b back-word         ", ".word_left", k_default },
	{ K_WORD_RIGHT   ,"K_WORD_RIGHT"   , "esc f forward-word      ", ".word_right", k_default },
	{ K_LINE_LEFT    ,"K_LINE_LEFT"    , "C-a beginning-of-line   ", ".line_left", k_default },
	{ K_LINE_RIGHT   ,"K_LINE_RIGHT"   , "C-e end-of-line         ", ".line_right", k_default },
	{ K_FILE_TOP     ,"K_FILE_TOP"     , "esc < beg-of-buf        ", ".file_top", k_default },
	{ K_FILE_BOTTOM  ,"K_FILE_BOTTOM"  , "esc > end-of-buf        ", ".file_bottom", k_default },
	//	{ K_HELP         ,"K_HELP"         , "                        ", ".help", k_default },
	//	{ K_HELP_OFF     ,"K_HELP_OFF"     , "                        ", ".help_off", k_token },
	//	{ K_HELP_TEXT    ,"K_HELP_TEXT"    , "                        ", ".help_text", k_help },
	{ K_MACRO        ,"K_MACRO"        , "                        ", ".macro", k_default },
	{ K_MACRO_DEFINE ,"K_MACRO_DEFINE" , "                        ", ".macro_define", k_default },
	{ K_QUIT         ,"K_QUIT"         , "C-x C-c                 ", ".quit", k_default },
	{ K_QUIT_ASK     ,"K_QUIT_ASK"     , "                        ", ".quit_ask", k_default },
	{ K_FILE_READ    ,"K_FILE_READ"    , "C-x C-f find-file       ", ".file_read", k_default },
	{ K_FILE_WRITE   ,"K_FILE_WRITE"   , "C-x C-d write-file      ", ".file_write", k_default },
	{ K_STTY_ERASE   ,"K_STTY_ERASE"   , "                        ", ".stty_erase", k_default },
	{ K_STTY_KILL    ,"K_STTY_KILL"    , "                        ", ".stty_kill", k_default },
	{ K_ITSELF       ,"K_ITSELF"       , "                        ", ".itself", k_default },
	{ K_REDRAW       ,"K_REDRAW"       , "C-l                     ", ".redraw", k_default },
	{ K_SHOW_VERSION ,"K_SHOW_VERSION" , "esc esc show-version    ", ".show_version", k_default },
	{ K_LITERAL      ,"K_LITERA"       , "                        ", ".literal", k_default },
	{ K_ERROR        ,"K_ERROR"        , "                        ", NULL, NULL }
};

// dummy
size_t encodekey(str, buf)
char *str, *buf;
{
	return 1;
}


/*
 * .function string
 *
 * Encode a key sequence for the given function.
 */
static int
k_default(fp, buf, kp)
FILE *fp;
char *buf;
keymap_t *kp;
{
        char *tok;
        size_t lhs;

        if ((tok = strtok(NULL, blank)) == NULL
			//|| (lhs = encodekey(tok, buf)) == 0
        || (kp->lhs = (char *) realloc(buf, lhs)) == NULL) {
                free(buf);
                return (FALSE);
        }

        return (TRUE);

}


/*
 * .stty_erase
 */
static int
k_erase(fp, buf, kp)
FILE *fp;
char *buf;
keymap_t *kp;
{
        buf[0] = erasechar();
        buf[1] = '\0';
        return ((kp->lhs = (char *) realloc(buf, 2)) != NULL);

}

/*
 * .stty_kill
 */
static int
k_kill(fp, buf, kp)
FILE *fp;
char *buf;
keymap_t *kp;
{
        buf[0] = killchar();
        buf[1] = '\0';
        return ((kp->lhs = (char *) realloc(buf, 2)) != NULL);

}


/*
 * Find token and return corresponding table entry; else NULL.
 */
keymap_t *
findkey(kp, token)
keymap_t *kp;
char *token;
{
        for (; kp->code != K_ERROR; ++kp)
                if (kp->lhs != NULL && strcmp(token, kp->lhs) == 0)
                        return (kp);
        return (NULL);

}

int
getkey(keys)
keymap_t *keys;
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
                        if (k->lhs == NULL
                        || k->code == K_DISABLED || k->code == K_HELP_TEXT)
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

int
getliteral()
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
static int
ipush(buf)
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
static int
ipop()
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

int
ismacro()
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

int
getinput(buf, len, echoing)
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

static int
fld_done()
{
        return (FALSE);

}

static int
fld_left()
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

static int
fld_erase()
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

static int
fld_kill()
{
        move(fld_row, fld_col);
        while (0 < fld_index--)
                addch(' ');
        move(fld_row, fld_col);
        fld_buffer[0] = '\0';
        fld_index = 0;
        return (TRUE);

}

static int
fld_insert()
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

keyinit_t *find_keyword(int code)
{
	keyinit_t *kw;
	
	for (kw = keywords; kw->code != K_ERROR; ++kw)
	{
		if (kw->code == code)
			return kw;
	}

	return NULL;
}
