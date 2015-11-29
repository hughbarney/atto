/*
 * display.c
 *
 * AttoEmacs, Hugh Barney, November 2015
 * Derived from: Anthony's Editor January 93, (Public Domain 1991, 1993 by Anthony Howe)
 *
 */

#include <ctype.h>
#include <string.h>
#include "header.h"

/* Reverse scan for start of logical line containing offset */
point_t lnstart(register point_t off)
{
	register char_t *p;
	do
		p = ptr(--off);
	while (curbp->b_buf < p && *p != '\n');
	return (curbp->b_buf < p ? ++off : 0);

}

/*
 * Forward scan for start of logical line segment containing 'finish'.
 * A segment of a logical line corresponds to a physical screen line.
 */
point_t segstart(point_t start, point_t finish)
{
	char_t *p;
	int c = 0;
	point_t scan = start;
	while (scan < finish) {
		p = ptr(scan);
		if (*p == '\n') {
			c = 0;
			start = scan+1;
		} else if (COLS <= c) {
			c = 0;
			start = scan;
		}
		++scan;
		c += *p == '\t' ? 8 - (c & 7) : 1;
	}
	return (c < COLS ? start : finish);
}

/* Forward scan for start of logical line segment following 'finish' */
point_t segnext(point_t start, point_t finish)
{
	char_t *p;
	int c = 0;
	point_t scan = segstart(start, finish);
	for (;;) {
		p = ptr(scan);
		if (curbp->b_ebuf <= p || COLS <= c)
			break;
		++scan;
		if (*p == '\n')
			break;
		c += *p == '\t' ? 8 - (c & 7) : 1;
	}
	return (p < curbp->b_ebuf ? scan : pos(curbp->b_ebuf));
}

/* Move up one screen line */
point_t upup(point_t off)
{
	point_t curr = lnstart(off);
	point_t seg = segstart(curr, off);
	if (curr < seg)
		off = segstart(curr, seg-1);
	else
		off = segstart(lnstart(curr-1), curr-1);
	return (off);
}

/* Move down one screen line */
point_t dndn(point_t off)
{
	return (segnext(lnstart(off), off));
}

/* Return the offset of a column on the specified line */
point_t lncolumn(point_t offset, int column)
{
	int c = 0;
	char_t *p;
	while ((p = ptr(offset)) < curbp->b_ebuf && *p != '\n' && c < column) {
		c += *p == '\t' ? 8 - (c & 7) : 1;
		++offset;
	}
	return (offset);
}

void display()
{
	char_t *p;
	int i, j;

	/* Re-frame the screen with the screen line containing the point
	 * as the first line, when point < page.  Handles the cases of a
	 * backward scroll or moving to the top of file.  pgup() will
	 * move page relative to point so that page <= point < epage.
	 */
	if (curbp->b_point < curbp->b_page)
		curbp->b_page = segstart(lnstart(curbp->b_point), curbp->b_point);
	/* Re-frame the whole screen when epage <= point.  Handles the
	 * cases of a forward scroll or redraw.
	 */
	if (curbp->b_epage <= curbp->b_point) {
		/* Find end of screen plus one. */
		curbp->b_page = dndn(curbp->b_point);
		/* Number of lines on the screen depends if we are at the
		 * EOF and how many lines are used for help and status.
		 */
		if (pos(curbp->b_ebuf) <= curbp->b_page) {
			curbp->b_page = pos(curbp->b_ebuf);
			i = LINES - 3;
		} else {
			i = LINES - 2;
		}
		i -= FIRST_LINE;
		/* Scan backwards the required number of lines. */
		while (0 < i--)
			curbp->b_page = upup(curbp->b_page);
	}

	move(FIRST_LINE, 0);
	i = FIRST_LINE;
	j = 0;
	curbp->b_epage = curbp->b_page;
	while (1) {
		if (curbp->b_point == curbp->b_epage) {
			row = i;
			col = j;
		}
		p = ptr(curbp->b_epage);
		if ((MAXLINE) <= i || curbp->b_ebuf <= p)
			break;
		if (*p != '\r') {
			if (isprint(*p) || *p == '\t' || *p == '\n') {
				j += *p == '\t' ? 8-(j&7) : 1;
				addch(*p);
			} else {
				const char *ctrl = unctrl(*p);
				j += (int) strlen(ctrl);
				addstr(ctrl);
			}
		}
		if (*p == '\n' || COLS <= j) {
			j -= COLS;
			if (j < 0)
				j = 0;
			++i;
		}
		++curbp->b_epage;
	}
	standend();
	clrtobot();
	modeline();
	dispmsg();
	move(row, col);
	refresh();
}

void modeline()
{
	int i;
	standout();
	move(MODELINE, 0);
	addch('=');
		
	addch(curbp->b_modified ? '*' : '=');
	addstr(" Atto: == ");
	addstr(get_buffer_name(curbp));
	addch(' ');
	i = 14 + strlen(get_buffer_name(curbp));
		
	for (; i<=COLS; i++)
		addch('=');
		
	standend();
}

void dispmsg()
{
	move(MSGLINE, 0);
	if (msgflag) {
		addstr(msgline);
		msgflag = FALSE;
	}
	clrtoeol();
}
