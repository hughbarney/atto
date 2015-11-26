/*
 * search.c
 *
 * AttoEmacs, Hugh Barney, November 2015, A single buffer, single screen Emacs
 * Simple forward and reverse search.
 */

#include <ctype.h>
#include <string.h>
#include "header.h"
#include "key.h"

#define FWD_SEARCH 1
#define REV_SEARCH 2

point_t search_forward(char *);
point_t search_backwards(char *);
void update_search_prompt(char *, char *);
void display_search_result(point_t, int, char *, char *);

void dosearch(char *prompt, char *search, int nsize)
{
	int cpos = 0;	
	int c;
	point_t o_point = point;
	point_t found;

	update_search_prompt(prompt, search);
	cpos = strlen(search);

	for (;;)
	{
		c = getch();
		/* ignore control keys other than C-g, backspace, CR,  C-s, C-R, ESC */
		if (c < 32 && c != 07 && c != 0x08 && c != 0x13 && c != 0x12 && c != 0x1b)
			continue;

		switch(c) {
		case 0x1b: /* esc */
			search[cpos] = '\0';
			return;

		case 0x07: /* ctrl-g */
			point = o_point;
			return;

		case 0x13: /* ctrl-s, do the search */
			found = search_forward(search);
			display_search_result(found, FWD_SEARCH, prompt, search);
			break;

		case 0x12: /* ctrl-r, do the search */
			found = search_backwards(search);
			display_search_result(found, REV_SEARCH, prompt, search);
			break;
			
		case 0x7f: /* del, erase */
		case 0x08: /* backspace */
			if (cpos == 0)
				continue;
			search[--cpos] = '\0';
			update_search_prompt(prompt, search);
			break;

		default:	
			if (cpos < nsize -1) {
				search[cpos++] = c;
				update_search_prompt(prompt, search);
			}
			break;
		}
	}
}

void display_search_result(point_t found, int dir, char *prompt, char *search)
{
	if (found != -1 ) {
		point = found;
		msg("%s%s",prompt, search);
		display();
	} else {
		msg("Failing %s%s",prompt, search);
		dispmsg();
		point = (dir == FWD_SEARCH ? 0 : pos(ebuf));
	}
}

void update_search_prompt(char *prompt, char *response)
{
	mvaddstr(MSGLINE, 0, prompt);
	/* if we have a value print it and go to end of it */
	if (response[0] != '\0')
		addstr(response);
	clrtoeol();
}

point_t search_forward(char *stext)
{
	point_t end_p = pos(ebuf);
	point_t p,pp;
	char* s;

	if (0 == strlen(stext))
		return point;

	for (p=point; p < end_p; p++) {
		for (s=stext, pp=p; *s == *(ptr(pp)) && *s !='\0' && pp < end_p; s++, pp++)
			;

		if (*s == '\0')
			return pp;
	}

	return -1;
}

point_t search_backwards(char *stext)
{
	point_t p,pp;
	char* s;
	
	if (0 == strlen(stext))
		return point;

	for (p=point; p > 0; p--) {
		for (s=stext, pp=p; *s == *(ptr(pp)) && *s != '\0' && pp > 0; s++, pp++)
			;
		
		if (*s == '\0') {
			if (p > 0)
				p--;
			return p;
		}
	}
	return -1;
}
