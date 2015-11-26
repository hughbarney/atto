/*
 * key.h
 *
 * AttoEmacs, Hugh Barney, November 2015, A single buffer, single screen Emacs
 * Derived from: Anthony's Editor January 93, (Public Domain 1991, 1993 by Anthony Howe)
 *
 */

#ifndef __key_h__
#define __key_h__       1

#include <stdio.h>
#include <sys/types.h>

#undef _
#define _(x)    x
#define K_BUFFER_LENGTH         256

typedef struct keymap_t {
	char *key_bind;
	char *lhs;              /* Left hand side invokes function or macro. */
	void (*func) _((void));
} keymap_t;

extern int getkey _((keymap_t *, keymap_t **));
extern int getinput _((char *, char *, int));

#endif /* __key_h__ */
