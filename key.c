/*
 * key.c
 *
 * AttoEmacs, Hugh Barney, November 2015, A single buffer, single screen Emacs
 * Derived from: Anthony's Editor January 93, (Public Domain 1991, 1993 by Anthony Howe)
 * 
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
			if (k->lhs == NULL) {
				continue;
			}
			for (p = buffer, q = k->lhs; *p == *q; ++p, ++q) {
				if (*q == '\0') {
					if (k->code == K_LITERAL)
						return (getliteral());
					if (k->code != K_MACRO_DEFINE) {
						/* Return extended key code. */
						return (k->code);
					}
					if (k->rhs != NULL) {
						(void) ipush(k->rhs);
						/* reset record and return 'we are a macro' */
						record = buffer;
						*record = '\0';
						return K_MACRO_DEFINE;
					}
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
	if (ch == EOF) {
		return ((unsigned) getch());
	}
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

int getinput(char *prompt, char *buf, int nbuf)
{
	int cpos = 0;
	int c;
	int start_col = strlen(prompt); 

	mvaddstr(MSGLINE, 0, prompt);
	clrtoeol();

	/* if we have a default value print it and go to end of it */
	if (buf[0] != '\0') {
		addstr(buf);
		cpos = strlen(buf);
	}

	for (;;)
    {
		refresh();
		c = getch();
		/* ignore control keys other than backspace, cr, lf */
		if (c < 32 && c != 0x08 && c != 0x0a && c != 0x0d)
			continue;

		switch(c) {
		case 0x0a: /* cr, lf */
		case 0x0d:
			buf[cpos] = '\0';
			return (cpos > 0 ? TRUE : FALSE);

		case 0x07: /* ctrl-g */
			return FALSE;

		case 0x7f: /* del, erase */
		case 0x08: /* backspace */
			if (cpos == 0)
				continue;
			
			move(MSGLINE, start_col + cpos - 1);
			addch(' ');
			move(MSGLINE, start_col + cpos - 1);
			buf[--cpos] = '\0';
			break;

		default:	
			if (cpos < nbuf -1) {
				addch(c);
				buf[cpos++] = c;
			}
			break;
		}
    }
}
