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

void prt_macros _((void));
int yesno _((int));
int more _((int));

void
top()
{
        point = 0;

}

void
bottom()
{
        epage = point = pos(ebuf);

}

void
quit_ask()
{
        if (modified) {
                standout();
                mvaddstr(MSGLINE, 0, str_notsaved);
                standend();
                clrtoeol();
                if (!yesno(FALSE))
                        return;
        }
        quit();

}

int
yesno(flag)
int flag;
{
        int ch;

        addstr(flag ? str_yes : str_no);
        refresh();
        ch = getliteral();
        if (ch == '\r' || ch == '\n')
                return (flag);
        return (ch == str_yes[1]);

}

void
quit()
{
        done = 1;

}

void
redraw()
{
        int col;
        keymap_t *kp;
        clear();
        if (textline != HELPLINE) {
                move(HELPLINE, 0);
                /* Display all the help text entries. */
                for (kp = key_map; kp->code != K_ERROR; ++kp)
                        if (kp->code == K_HELP_TEXT)
                                addstr(kp->lhs);
                ruler(COLS);
                getyx(stdscr, textline, col);
        }
        display();

}

void
left()
{
        if (0 < point)
                --point;

}

void
right()
{
        if (point < pos(ebuf))
                ++point;

}

void
up()
{
        point = lncolumn(upup(point), col);

}

void
down()
{
        point = lncolumn(dndn(point), col);

}

void
lnbegin()
{
        point = segstart(lnstart(point), point);

}

void
lnend()
{
        point = dndn(point);
        left();

}

void
wleft()
{
        char_t *p;
        while (!isspace(*(p = ptr(point))) && buf < p)
                --point;
        while (isspace(*(p = ptr(point))) && buf < p)
                --point;

}

void
pgdown()
{
        page = point = upup(epage);
        while (textline < row--)
                down();
        epage = pos(ebuf);

}

void
pgup()
{
        int i = LINES;
        while (textline < --i) {
                page = upup(page);
                up();
        }

}

void
wright()
{
        char_t *p;
        while (!isspace(*(p = ptr(point))) && p < ebuf)
                ++point;
        while (isspace(*(p = ptr(point))) && p < ebuf)
                ++point;

}

void
insert()
{
        assert(gap <= egap);
        if (gap == egap && !growgap(CHUNK))
                return;
        point = movegap(point);
        *gap++ = input == '\r' ? '\n' : input;
        modified = TRUE;
        point = pos(egap);

}

void
insert_mode()
{
        int ch;
        point_t opoint;
        point = opoint = movegap(point);
        undoset();
        while ((ch = getkey(key_mode)) != K_INSERT_EXIT) {
                if (ch == K_STTY_ERASE) {
                        if (opoint < point) {
                                --gap;
                                modified = TRUE;
                        }
                } else {
                        assert(gap <= egap);
                        if (gap == egap) {
                                point = pos(egap);
                                if (!growgap(CHUNK))
                                        break;
                        }
                        *gap++ = ch == '\r' ? '\n' : ch;
                        modified = TRUE;
                }
                point = pos(egap);
                display();
        }

}

void
backsp()
{
        point = movegap(point);
        undoset();
        if (buf < gap) {
                --gap;
                modified = TRUE;
        }
        point = pos(egap);

}

void
delete()
{
        point = movegap(point);
        undoset();
        if (egap < ebuf) {
                point = pos(++egap);
                modified = TRUE;
        }

}

void
readfile()
{
        standout();
        mvaddstr(MSGLINE, 0, str_read);
        standend();
        clrtoeol();
        addch(' ');
        refresh();
        temp[0] = '\0';
        getinput((char*) temp, BUFSIZ, TRUE);
        if (temp[0] != '\0')
                (void) load(temp);

}

void
writefile()
{
        standout();
        mvaddstr(MSGLINE, 0, str_write);
        standend();
        clrtoeol();
        addch(' ');
        refresh();
        strcpy(temp, filename);
        getinput((char*) temp, BUFSIZ, TRUE);
        if (temp[0] != '\0')
                (void) save(temp);

}

void
help()
{
        textline = textline == HELPLINE ? -1 : HELPLINE;
        /* When textline != HELPLINE, then redraw() will compute the
         * actual textline that follows the help text.
         */
        redraw();

}

void
block()
{
        marker = marker == NOMARK ? point : NOMARK;

}

void
cut()
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

void
paste()
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

void
version()
{
        msg(m_version);

}

void
macro()
{
        keymap_t *kp;
        int lhs_len, rhs_len;
        char *buf, *lhs, *rhs;

        if ((buf = (char *) malloc(BUFSIZ)) == NULL) {
                msg(m_alloc);
                return;
        }
        standout();
        mvaddstr(MSGLINE, 0, str_macro);
        standend();
        clrtoeol();
        addch(' ');
        refresh();
        buf[0] = '\0';
        getinput(buf, BUFSIZ, TRUE);
        if ((lhs = strtok(buf, " \t")) == NULL) {
                prt_macros();
				/*
        } else if (0 == (lhs_len = encodekey(lhs, buf))) {
                msg(m_lhsbad);
				*/
        } else {
                kp = findkey(key_map, lhs);
                if ((rhs = strtok(NULL, " \t")) == NULL) {
                        /* Delete macro. */
                        if (kp == NULL || kp->code != K_MACRO_DEFINE) {
                                msg(m_nomacro);
                        } else {
                                free(kp->lhs);
                                kp->lhs = kp->rhs = NULL;
                        }
						/*
                } else if (0 == (rhs_len = encodekey(rhs, buf+lhs_len+1))) {
                        msg(m_rhsbad);
						*/
                } else {
                        /* Assume that shrinking succeeds. */
                        lhs = (char *) realloc(buf, lhs_len+rhs_len+2);
                        if (kp == NULL) {
                                /* Find free slot to add macro. */
                                for (kp = key_map; kp->code != K_ERROR; ++kp) {
                                        if (kp->code == K_MACRO_DEFINE
                                        && kp->lhs == NULL)
                                                break;
                                }
                        }
                        if (kp->code == K_ERROR) {
                                free(lhs);
                                msg(m_slots);
                        } else if (kp->code == K_MACRO_DEFINE) {
                                /* Change macro. */
                                kp->lhs = lhs;
                                kp->rhs = lhs+lhs_len+1;
                                return;
                        } else {
                                msg(m_nomacro);
                        }
                }
        }
        free(buf);

}

void
prt_macros()
{
        char *ptr;
        keymap_t *kp;
        int used, total;

        erase();
        scrollok(stdscr, TRUE);
        for (used = total = 0, kp = key_map; kp->code != K_ERROR; ++kp) {
                if (kp->code == K_MACRO_DEFINE) {
                        ++total;
                        if (kp->rhs != NULL) {
                                ++used;
                                addstr(kp->lhs);
                                addch('\t');
                                for (ptr = kp->rhs; *ptr != '\0'; ++ptr) {
                                        if (isprint(*ptr))
                                                addch(*ptr);
                                        else
                                                addstr(unctrl(*ptr));
                                }
                                addch('\n');
                                (void) more(used);
                        }
                }
        }
        printw("\n%d/%d\n", used, total, str_press);
        scrollok(stdscr, FALSE);
        (void) more(-1);
        redraw();

}

int
more(row)
int row;
{
        int ch;

        if (0 < row % (LINES-1))
                return (TRUE);
        standout();
        addstr(str_more);
        standend();
        clrtoeol();
        refresh();
        ch = getliteral();
        addch('\r');
        clrtoeol();
        return (ch != str_quit[1] && ch != str_no[1]);

}

