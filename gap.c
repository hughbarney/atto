/*
 * gap.c                
 *
 * Anthony's Editor January 93
 *
 * Public Domain 1991, 1993 by Anthony Howe.  No warranty.
 */

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "header.h"

typedef struct undo_t {
	point_t u_point;
	point_t u_gap;
	point_t u_egap;

} undo_t;

static undo_t ubuf;

/*
 *      Enlarge the gap by n characters.  
 *      Note that the position of the gap cannot change.
 */
int growgap(point_t n)
{
	char_t *new;
	point_t buflen, newlen, xgap, xegap;

	//debug_stats("growgap() START");
		
	assert(buf <= gap);
	assert(gap <= egap);
	assert(egap <= ebuf);

	xgap = gap - buf;
	xegap = egap - buf;
	buflen = ebuf - buf;
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
		new = (char_t*) realloc(buf, (size_t) newlen);
		if (new == NULL) {
			/* Report non-fatal error. */
			msg(m_alloc);
			return (FALSE);
		}
	}

	/* Relocate pointers in new buffer and append the new
	 * extension to the end of the gap.
	 */
	buf = new;
	gap = buf + xgap;      
	ebuf = buf + buflen;
	egap = buf + newlen;
	while (xegap < buflen--)
		*--egap = *--ebuf;
	ebuf = buf + newlen;

	assert(buf < ebuf);          /* Buffer must exist. */
	assert(buf <= gap);
	assert(gap < egap);          /* Gap must grow only. */
	assert(egap <= ebuf);

	//debug_stats("growgap() END");

	return (TRUE);
}

point_t movegap(point_t offset)
{
	char_t *p = ptr(offset);
	while (p < gap)
		*--egap = *--gap;
	while (egap < p)
		*gap++ = *egap++;
	assert(gap <= egap);
	assert(buf <= gap);
	assert(egap <= ebuf);
	return (pos(egap));
}

/* Given a buffer offset, convert it to a pointer into the buffer */
char_t * ptr(register point_t offset)
{
	if (offset < 0)
		return (buf);
	return (buf+offset + (buf+offset < gap ? 0 : egap-gap));
}

/*
 *      Given a pointer into the buffer, convert it to a buffer offset.
 */
point_t pos(register char_t *cp)
{
	assert(buf <= cp && cp <= ebuf);
	return (cp-buf - (cp < egap ? 0 : egap-gap));
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

	//debug("save()\n");
		
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
	length = (point_t) (ebuf-egap);
	if (fwrite(egap, sizeof (char), (size_t) length, fp) != length) {
		msg(m_write, fn);
		return (FALSE);
	}
	if (fclose(fp) != 0) {
		msg(m_close, fn);
		return (FALSE);
	}
	modified = FALSE;
	msg(m_saved, fn, pos(ebuf));
	return (TRUE);
}

int load_file(char *fn)
{
	/* reset the gap, make it the whole buffer */
	gap = buf;
	egap = ebuf;
	top();
	return insert_file(fn, FALSE);
}

/* reads file into buffer at point */
int insert_file(char *fn, int modflag)
{
	FILE *fp;
	size_t len;
	struct stat sb;

	//debug_stats("load() START: ");

	if (stat(fn, &sb) < 0) {
		msg(m_stat, fn);
		return (FALSE);
	}
	if (MAX_SIZE_T < sb.st_size) {
		msg(m_toobig, fn);
		return (FALSE);
	}
	if (egap-gap < sb.st_size * sizeof (char_t) && !growgap(sb.st_size))
		return (FALSE);
	if ((fp = fopen(fn, "r")) == NULL) {
		msg(m_open, fn);
		return (FALSE);
	}
	point = movegap(point);
	undoset();
	gap += len = fread(gap, sizeof (char), (size_t) sb.st_size, fp);

	//debug_stats("load() END: ");

	if (fclose(fp) != 0) {
		msg(m_close, fn);
		return (FALSE);
	}
	modified = modflag;
	msg(m_loaded, fn, len);
	return (TRUE);
}

/* Record a new undo location */
void undoset()
{
	ubuf.u_point = point;
	ubuf.u_gap = gap - buf;
	ubuf.u_egap = egap - buf;
}

/* Undo */
void undo()
{
	undo_t tmp;
	memcpy(&tmp, &ubuf, sizeof (undo_t));
	undoset();
	point = tmp.u_point;
	gap = buf + tmp.u_gap;
	egap = buf + tmp.u_egap;
	modified = TRUE;
}

