/*
 * gap.c                
 *
 * AttoEmacs, Hugh Barney, November 2015
 * Derived from: Anthony's Editor January 93, (Public Domain 1991, 1993 by Anthony Howe)
 *
 */

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "header.h"

/*
 *      Enlarge the gap by n characters.  
 *      Note that the position of the gap cannot change.
 */
int growgap(point_t n)
{
	char_t *new;
	point_t buflen, newlen, xgap, xegap;
		
	assert(curbp->b_buf <= curbp->b_gap);
	assert(curbp->b_gap <= curbp->b_egap);
	assert(curbp->b_egap <= curbp->b_ebuf);

	xgap = curbp->b_gap - curbp->b_buf;
	xegap = curbp->b_egap - curbp->b_buf;
	buflen = curbp->b_ebuf - curbp->b_buf;
	newlen = buflen + n * sizeof (char_t);

	if (buflen == 0) {
		if (newlen < 0 || MAX_SIZE_T < newlen)
			fatal(f_alloc);
		new = (char_t*) malloc((size_t) newlen);
		if (new == NULL)
			/* Cannot edit a file without a buffer. */
			fatal(f_alloc);
	} else {
		if (newlen < 0 || MAX_SIZE_T < newlen) {
			msg(m_alloc);
			return (FALSE);
		}
		new = (char_t*) realloc(curbp->b_buf, (size_t) newlen);
		if (new == NULL) {
			/* Report non-fatal error. */
			msg(m_alloc);
			return (FALSE);
		}
	}

	/* Relocate pointers in new buffer and append the new
	 * extension to the end of the gap.
	 */
	curbp->b_buf = new;
	curbp->b_gap = curbp->b_buf + xgap;      
	curbp->b_ebuf = curbp->b_buf + buflen;
	curbp->b_egap = curbp->b_buf + newlen;
	while (xegap < buflen--)
		*--curbp->b_egap = *--curbp->b_ebuf;
	curbp->b_ebuf = curbp->b_buf + newlen;

	assert(curbp->b_buf < curbp->b_ebuf);          /* Buffer must exist. */
	assert(curbp->b_buf <= curbp->b_gap);
	assert(curbp->b_gap < curbp->b_egap);          /* Gap must grow only. */
	assert(curbp->b_egap <= curbp->b_ebuf);
	return (TRUE);
}

point_t movegap(point_t offset)
{
	char_t *p = ptr(offset);
	while (p < curbp->b_gap)
		*--curbp->b_egap = *--curbp->b_gap;
	while (curbp->b_egap < p)
		*curbp->b_gap++ = *curbp->b_egap++;
	assert(curbp->b_gap <= curbp->b_egap);
	assert(curbp->b_buf <= curbp->b_gap);
	assert(curbp->b_egap <= curbp->b_ebuf);
	return (pos(curbp->b_egap));
}

/* Given a buffer offset, convert it to a pointer into the buffer */
char_t * ptr(register point_t offset)
{
	if (offset < 0)
		return (curbp->b_buf);
	return (curbp->b_buf+offset + (curbp->b_buf + offset < curbp->b_gap ? 0 : curbp->b_egap-curbp->b_gap));
}

/*
 *      Given a pointer into the buffer, convert it to a buffer offset.
 */
point_t pos(register char_t *cp)
{
	assert(curbp->b_buf <= cp && cp <= curbp->b_ebuf);
	return (cp - curbp->b_buf - (cp < curbp->b_egap ? 0 : curbp->b_egap - curbp->b_gap));
}

int posix_file(char *fn)
{
	if (fn[0] == '_')
		return (FALSE);

	for (; *fn != '\0'; ++fn) {
		if (!isalnum(*fn) && *fn != '.' && *fn != '_' && *fn != '-' && *fn != '/')
			return (FALSE);
	}
	return (TRUE);
}

int save(char *fn)
{
	FILE *fp;
	point_t length;
		
	if (!posix_file(fn)) {
		msg(m_badname);
		return (FALSE);
	}
	fp = fopen(fn, "w");
	if (fp == NULL) {
		msg(m_open, fn);
		return (FALSE);
	}
	(void) movegap((point_t) 0);
	length = (point_t) (curbp->b_ebuf - curbp->b_egap);
	if (fwrite(curbp->b_egap, sizeof (char), (size_t) length, fp) != length) {
		msg(m_write, fn);
		return (FALSE);
	}
	if (fclose(fp) != 0) {
		msg(m_close, fn);
		return (FALSE);
	}
	curbp->b_modified = FALSE;
	msg(m_saved, fn, pos(curbp->b_ebuf));
	return (TRUE);
}

int load_file(char *fn)
{
	/* reset the gap, make it the whole buffer */
	curbp->b_gap = curbp->b_buf;
	curbp->b_egap = curbp->b_ebuf;
	top();
	return insert_file(fn, FALSE);
}

/* reads file into buffer at point */
int insert_file(char *fn, int modflag)
{
	FILE *fp;
	size_t len;
	struct stat sb;

	if (stat(fn, &sb) < 0) {
		msg(m_stat, fn);
		return (FALSE);
	}
	if (MAX_SIZE_T < sb.st_size) {
		msg(m_toobig, fn);
		return (FALSE);
	}
	if (curbp->b_egap - curbp->b_gap < sb.st_size * sizeof (char_t) && !growgap(sb.st_size))
		return (FALSE);
	if ((fp = fopen(fn, "r")) == NULL) {
		msg(m_open, fn);
		return (FALSE);
	}
	curbp->b_point = movegap(curbp->b_point);
	undoset();
	curbp->b_gap += len = fread(curbp->b_gap, sizeof (char), (size_t) sb.st_size, fp);

	if (fclose(fp) != 0) {
		msg(m_close, fn);
		return (FALSE);
	}
	curbp->b_modified = modflag;
	msg(m_loaded, fn, len);
	return (TRUE);
}

/* Record a new undo location */
void undoset()
{
	curbp->b_ubuf.u_point = curbp->b_point;
	curbp->b_ubuf.u_gap = curbp->b_gap - curbp->b_buf;
	curbp->b_ubuf.u_egap = curbp->b_egap - curbp->b_buf;
}

/* Undo */
void undo()
{
	undo_t tmp;
	memcpy(&tmp, &(curbp->b_ubuf), sizeof (undo_t));
	undoset();
	curbp->b_point = tmp.u_point;
	curbp->b_gap = curbp->b_buf + tmp.u_gap;
	curbp->b_egap = curbp->b_buf + tmp.u_egap;
	curbp->b_modified = TRUE;
}
