/*
 * command.c            
 *
 * Anthony's Editor January 93
 *
 * Public Domain 1991, 1993 by Anthony Howe.  No warranty.
 */

#include <ctype.h>
#include <string.h>
#include "header.h"

int yesno _((int));

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
	mvaddstr(MSGLINE, 0, str_insert_file);
	clrtoeol();
	addch(' ');
	refresh();
	temp[0] = '\0';
	getinput((char*) temp, BUFSIZ, TRUE);
	if (temp[0] != '\0')
		(void) insert_file(temp, TRUE);
}

void readfile()
{
	/* if modified as if want to save changes */
	if (modified) {
		mvaddstr(MSGLINE, 0, str_querysave);
		clrtoeol();
		if (yesno(TRUE)) {
			savebuffer();
			modeline();
			refresh();
        }
	}
	
	mvaddstr(MSGLINE, 0, str_read);
	clrtoeol();
	addch(' ');
	refresh();
	temp[0] = '\0';
	getinput((char*) temp, BUFSIZ, TRUE);
	if (temp[0] != '\0')
		(void) load_file(temp);
}

void savebuffer()
{
	//debug("savebuffer()\n");
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
	mvaddstr(MSGLINE, 0, str_write);
	clrtoeol();
	addch(' ');
	refresh();
	strcpy(temp, filename);
	getinput((char*) temp, BUFSIZ, TRUE);
	if (temp[0] != '\0')
		if (save(temp) == TRUE)
			strcpy(filename, temp);
}

void block()
{
        marker = marker == NOMARK ? point : NOMARK;
}

void cut()
{
        char_t *p;
        if (marker == NOMARK || point == marker)
                return;
        if (scrap != NULL) {
                free(scrap);
                scrap = NULL;
        }
        if (point < marker) {
                p = ptr(point);
                (void) movegap(point);
                nscrap = marker - point;
        } else {
                p = ptr(marker);
                (void) movegap(marker);
                nscrap = point - marker;
        }
        if ((scrap = (char_t*) malloc(nscrap)) == NULL) {
                msg(m_alloc);
        } else {
                undoset();
                (void) memcpy(scrap, p, nscrap * sizeof (char_t));
                egap += nscrap;
                block();
                point = pos(egap);
                modified = TRUE;
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
