/*
 * buffer.c                
 *
 * AttoEmacs, Hugh Barney, November 2015
 *
 */

#include <assert.h>
#include <string.h>
#include "header.h"

void buffer_init(buffer_t *bp)
{
	bp->b_mark = 0;
	bp->b_point = 0;
	bp->b_page = 0;
	bp->b_epage = 0;
	bp->b_modified = FALSE;
	bp->b_buf = NULL;
	bp->b_ebuf = NULL;
	bp->b_gap = NULL;
	bp->b_egap = NULL;
	bp->b_next = NULL;
	bp->b_fname[0] = '\0';
}

/*
 * Find a buffer, by filename. Return a pointer to the buffer_t
 * structure associated with it. If the buffer is not found and the
 * "cflag" is TRUE, create it.
 */
buffer_t* find_buffer (char *fname, int cflag)
{
	buffer_t *bp = NULL;
	buffer_t *sb = NULL;
	
	bp = bheadp;
	while (bp != NULL)
    {
		if (strcmp (fname, bp->b_fname) == 0 || strcmp(fname, bp->b_bname) == 0) {
			return (bp);
		}
		bp = bp->b_next;
    }

	if (cflag != FALSE)
    {
		if ((bp = (buffer_t *) malloc (sizeof (buffer_t))) == NULL)
			return (0);

		buffer_init(bp);
		assert(bp != NULL);
		
		/* find the place in the list to insert this buffer */
		if (bheadp == NULL) {
			bheadp = bp;
		}
        else if (strcmp (bheadp->b_fname, fname) > 0)
		{
			/* insert at the begining */
			bp->b_next = bheadp;
			bheadp = bp;
		}
		else
		{
			for (sb = bheadp; sb->b_next != NULL; sb = sb->b_next)
				if (strcmp (sb->b_next->b_fname, fname) > 0)
					break;
			
			/* and insert it */
			bp->b_next = sb->b_next;
			sb->b_next = bp;
		}
    }
	return bp;
}

/*
 * Unlink from the list of buffers
 * Free the memory associated with the buffer
 * assumes that buffer has been saved if modified
 */
int delete_buffer (buffer_t *bp)
{
	buffer_t *sb = NULL;

	/* we must have switched to a different buffer first */
	assert(bp != curbp);
	
	/* if buffer is the head buffer */
	if (bp == bheadp) {
		bheadp = bp->b_next;
	}
	else
	{
		/* find place where the bp buffer is next */
		for (sb = bheadp; sb->b_next != bp && sb->b_next != NULL; sb = sb->b_next)
			;
		assert(sb->b_next == bp || sb->b_next == NULL);
		sb->b_next = bp->b_next;
	}

	/* now we can delete */
	free(bp->b_buf);
	free(bp);
	return TRUE;
}

void next_buffer()
{
	//window_t *wp;
	assert(curbp != NULL);
	assert(bheadp != NULL);

	// only do this if buffer not displayed elsewhere
	if ((winp1->w_bufp == curwp->w_bufp && curwp != winp1) || (winp2->w_bufp == curwp->w_bufp && curwp != winp2)) {
		debug("BUFFER NOT DISPLAYED ELSEWARE - Safe to save\n");
        w2b(curwp);
	}

	curbp = (curbp->b_next != NULL ? curbp->b_next : bheadp);
	curwp->w_bufp = curbp; // curwp to point to new buffer
	b2w(curwp); // retrieve the buffer settings
}

void prev_buffer()
{
	buffer_t *bp;
	int i = 0;
	assert(curbp != NULL);
	assert(bheadp != NULL);

	/* if current is the head find the end, otheriwse find bp where b_next == curbp */
	if (curbp == bheadp)
	{
		for (bp=bheadp; bp->b_next != NULL; bp = bp->b_next)
			assert(++i < 500);
	}
	else
	{
		for (bp=bheadp; bp->b_next != curbp; bp = bp->b_next)
			assert(++i < 500);
	}

	curbp=bp;
	curwp->w_bufp = curbp;
	curwp->w_point = curbp->b_point;
}

char* get_buffer_name(buffer_t *bp)
{
	return (strlen(bp->b_fname) > 0) ? bp->b_fname : bp->b_bname;
}

int count_buffers()
{
	buffer_t* bp;
	int i;

	for (i=1, bp=bheadp; bp != NULL; bp = bp->b_next)
		i++;

	return i;
}

int modified_buffers()
{
	buffer_t* bp;

	for (bp=bheadp; bp != NULL; bp = bp->b_next)
		if (bp->b_modified)
			return TRUE;

	return FALSE;
}
