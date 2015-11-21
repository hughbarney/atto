/*
 * command.c            
 *
 * AttoEmacs, Hugh Barney, November 2015, A single buffer, single screen Emacs
 * Derived from: Anthony's Editor January 93, (Public Domain 1991, 1993 by Anthony Howe)
 *
 */

#include <ctype.h>
#include <string.h>
#include "header.h"

void top()
{
	point = 0;
}

void bottom()
{
	epage = point = pos(ebuf);
}

void quit_ask()
{
	if (modified) {
		mvaddstr(MSGLINE, 0, str_notsaved);
		clrtoeol();
		if (!yesno(FALSE))
			return;
	}
	quit();
}

/* flag = default answer, FALSE=n, TRUE=y */
int yesno(int flag)
{
	int ch;

	addstr(flag ? str_yes : str_no);
	refresh();
	ch = getliteral();
	if (ch == '\r' || ch == '\n')
		return (flag);
	return (ch == str_yes[1]);

}

void quit()
{
	done = 1;
}

void redraw()
{
	clear();
	display();
}

void left()
{
	if (0 < point)
		--point;
}

void right()
{
	if (point < pos(ebuf))
		++point;
}

void up()
{
	point = lncolumn(upup(point), col);
}

void down()
{
	point = lncolumn(dndn(point), col);
}

void lnbegin()
{
	point = segstart(lnstart(point), point);
}

void lnend()
{
	point = dndn(point);
	left();
}

void wleft()
{
	char_t *p;
	while (!isspace(*(p = ptr(point))) && buf < p)
		--point;
	while (isspace(*(p = ptr(point))) && buf < p)
		--point;
}

void pgdown()
{
	page = point = upup(epage);
	while (FIRST_LINE < row--)
		down();
	epage = pos(ebuf);
}

void pgup()
{
	int i = LINES;
	while (FIRST_LINE < --i) {
		page = upup(page);
		up();
	}
}

void wright()
{
	char_t *p;
	while (!isspace(*(p = ptr(point))) && p < ebuf)
		++point;
	while (isspace(*(p = ptr(point))) && p < ebuf)
		++point;
}

void insert()
{
	assert(gap <= egap);
	if (gap == egap && !growgap(CHUNK))
		return;
	point = movegap(point);
	*gap++ = input == '\r' ? '\n' : input;
	modified = TRUE;
	point = pos(egap);
}

void backsp()
{
	point = movegap(point);
	undoset();
	if (buf < gap) {
		--gap;
		modified = TRUE;
	}
	point = pos(egap);
}

void delete()
{
	point = movegap(point);
	undoset();
	if (egap < ebuf) {
		point = pos(++egap);
		modified = TRUE;
	}
}

void insertfile()
{
	temp[0] = '\0';
	getinput(str_insert_file, (char*) temp, BUFSIZ);
	if (temp[0] != '\0')
		(void) insert_file(temp, TRUE);
}

void readfile()
{
	/* if modified and if want to save changes */
	if (modified) {
		mvaddstr(MSGLINE, 0, str_querysave);
		clrtoeol();
		if (yesno(TRUE)) {
			savebuffer();
			modeline();
			refresh();
		}
	}

	temp[0] = '\0';
	getinput(str_read, (char*) temp, BUFSIZ);
	if (temp[0] != '\0')
		if (load_file(temp) == TRUE)
			strcpy(filename, temp);
}

void savebuffer()
{
	if (filename[0] != '\0') {
		save(filename);
		return;
	} else {
		writefile();
	}
	refresh();
}

void writefile()
{
	strcpy(temp, filename);
	getinput(str_write, (char*)temp, BUFSIZ);
	if (temp[0] != '\0')
		if (save(temp) == TRUE)
			strcpy(filename, temp);
}

void iblock()
{
	block();
	msg(str_mark);
}

void block()
{
	marker = marker == NOMARK ? point : NOMARK;
}

void copy() {
	copy_cut(FALSE);
}

void cut() {
	copy_cut(TRUE);
}

void copy_cut(int cut)
{
	char_t *p;
	/* if no mark or point == marker, nothing doing */
	if (marker == NOMARK || point == marker)
		return;
	if (scrap != NULL) {
		free(scrap);
		scrap = NULL;
	}
	if (point < marker) {
		/* point above marker: move gap under point, region = marker - point */
		p = ptr(point);
		(void) movegap(point);
		nscrap = marker - point;
	} else {
		/* if point below marker: move gap under marker, region = point - marker */
		p = ptr(marker);
		(void) movegap(marker);
		nscrap = point - marker;
	}
	if ((scrap = (char_t*) malloc(nscrap)) == NULL) {
		msg(m_alloc);
	} else {
		undoset();
		(void) memcpy(scrap, p, nscrap * sizeof (char_t));
		if (cut) {
			egap += nscrap; /* if cut expand gap down */
			block();
			point = pos(egap); /* set point to after region */
			modified = TRUE;
		} else {
			block(); /* can maybe do without */
		}
	}
}

void paste()
{
	if (nscrap <= 0) {
		msg(m_scrap);
	} else if (nscrap < egap-gap || growgap(nscrap)) {
		point = movegap(point);
		undoset();
		memcpy(gap, scrap, nscrap * sizeof (char_t));
		gap += nscrap;
		point = pos(egap);
		modified = TRUE;
	}
}

void version()
{
	msg(m_version);
}

void macro_noop()
{
}
