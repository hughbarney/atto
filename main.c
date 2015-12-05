/*
 * main.c              
 *
 * AttoEmacs, Hugh Barney, November 2015
 * Derived from: Anthony's Editor January 93, (Public Domain 1991, 1993 by Anthony Howe)
 */

#include <string.h>
#include "header.h"

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

	winp1= &win1;
	winp2= &win2;
	init_window(winp1);
	one_window(winp1);
	wheadp = curwp = winp1;
	
	raw();
	noecho();
	idlok(stdscr, TRUE);
		
	if (1 < argc) {
		curbp = find_buffer(argv[1], TRUE);
		(void) insert_file(argv[1], FALSE);
		/* Save filename irregardless of load() success. */
		strcpy(curbp->b_fname, argv[1]);
	} else {
		curbp = find_buffer(str_scratch, TRUE);
		strcpy(curbp->b_bname, str_scratch);
	}

	curwp->w_bufp = curbp;
	b2w(curwp);

	if (!growgap(curbp, CHUNK))
		fatal(f_alloc);

	top();
	key_map = keymap;

	while (!done) {
		update_display();
		input = getkey(key_map, &key_return);

		if (key_return != NULL) {
			debug("\n\nI:%s: wp%d bp%d r%d c%d\n", curwp->w_name, curwp->w_point, curbp->b_point, curwp->w_row, curwp->w_col);
			//curbp->b_point = curwp->w_point;
			(key_return->func)();
			debug("O:%s: wp%d bp%d r%d c%d\n", curwp->w_name, curwp->w_point, curbp->b_point, curwp->w_row, curwp->w_col);
			b2w(curwp);
		}
		else
		  insert();
		//debug_stats("main loop:");
		//debug("main loop: row=%d,col=%d\n", row, col);
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
	debug("%s bsz=%d gap=%d egap=%d\n", s, curbp->b_ebuf - curbp->b_buf, curbp->b_gap - curbp->b_buf, curbp->b_egap - curbp->b_buf);
}
