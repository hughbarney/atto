/*
 * main.c, Atto Emacs, Hugh Barney, Public Domain, 2015
 * Derived from: Anthony's Editor January 93, (Public Domain 1991, 1993 by Anthony Howe)
 */

#include "header.h"

int done;
int result;
point_t nscrap;
char_t *scrap;

int input;
int msgflag;
char msgline[TEMPBUF];
char temp[TEMPBUF];
char searchtext[STRBUF_M];
char replace[STRBUF_M];

keymap_t *key_return;
keymap_t *key_map;
buffer_t *curbp;			/* current buffer */
buffer_t *bheadp;			/* head of list of buffers */
window_t *curwp;
window_t *wheadp;

int main(int argc, char **argv)
{
	if (initscr() == NULL)
		fatal("%s: Failed to initialize the screen.\n");

	raw();
	noecho();
	idlok(stdscr, TRUE);
		
	if (1 < argc) {
		curbp = find_buffer(argv[1], TRUE);
		(void) insert_file(argv[1], FALSE);
		/* Save filename irregardless of load() success. */
		strncpy(curbp->b_fname, argv[1], NAME_MAX);
		curbp->b_fname[NAME_MAX] = '\0'; /* force truncation */
	} else {
		curbp = find_buffer("*scratch*", TRUE);
		strncpy(curbp->b_bname, "*scratch*", STRBUF_S);
	}

	wheadp = curwp = new_window();
	one_window(curwp);
	associate_b2w(curbp, curwp);

	if (!growgap(curbp, CHUNK))
		fatal("%s: Failed to allocate required memory.\n");

	top();
	key_map = keymap;

	while (!done) {
		update_display();
		input = getkey(key_map, &key_return);

		if (key_return != NULL)
			(key_return->func)();
		else
			insert();
		/* debug_stats("main loop:"); */
	}
	if (scrap != NULL)
		free(scrap);

	move(LINES-1, 0);
	refresh();
	noraw();
	endwin();

	return 0;
}

void fatal(char *msg)
{
	if (curscr != NULL) {
		move(LINES-1, 0);
		refresh();
		endwin();
		putchar('\n');
	}
	fprintf(stderr, msg, PROG_NAME);
	exit(1);
}

void msg(char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	(void)vsprintf(msgline, msg, args);
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
	debug("%s bsz=%d gap=%d egap=%d\n", s, curbp->b_ebuf - curbp->b_buf, curbp->b_gap - curbp->b_buf, curbp->b_egap - curbp->b_buf);
}
