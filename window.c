#include "header.h"

int one_window(window_t *wp)
{
	wp->w_top = 0;
	wp->w_rows = LINES - 2;
	wp->w_bufp = curbp;
	strcpy(wp->w_name, "W1:");
	wp->w_displayed = TRUE;
	return TRUE;
}

void split_window()
{
	window_t *wp;
	int ntru, ntrl;

	if (curwp->w_rows < 3)
    {
		msg("Cannot split a %d line window", curwp->w_rows);
		return;
    }

	/* malloc here */
	wp = winp2;
	init_window(wp);
	wp->w_displayed = TRUE;
	strcpy(wp->w_name, "W2:");
	curwp->w_next = wp;

	//++curbp->b_nwnd;		/* Displayed twice */
	wp->w_bufp = winp1->w_bufp;

	b2w(wp); /* inherit buffer settings, row will be wrong */
  
	ntru = (curwp->w_rows - 1) / 2; /* Upper size */
	ntrl = (curwp->w_rows - 1) - ntru; /* Lower size */

    /* Old is upper window */
	curwp->w_rows = ntru;
	wp->w_top = curwp->w_top + ntru + 1;
	wp->w_rows = ntrl;

	//wp->w_row += wp->w_top; // correct row

	//display(curwp);
	//curwp = wp;
}

void init_window(window_t *wp)
{
	wp->w_next = NULL;
	wp->w_bufp = NULL;
	wp->w_point = 0;
	wp->w_mark = NOMARK;
	wp->w_top = 0;	
	wp->w_rows = 0;
	wp->w_displayed = FALSE;
	wp->w_name[0] = '\0';
}

void next_window() {
	window_t *old, *new;

	b2w(curwp); /* save our buffer vars in the win */

	old = curwp;
	curwp = new = (curwp == winp1 ? winp2 : winp1);
	curbp = curwp->w_bufp;

	if (new->w_bufp == old->w_bufp)
		w2b(curwp); /* push win vars to buffer */
	else
		b2w(curwp);
}
