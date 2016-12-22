/* hlite.c, generic syntax hilighting, Atto Emacs, Hugh Barney, Public Domain, 2016 */

#include "header.h"

int state = ID_DEFAULT;
int next_state = ID_DEFAULT;
int skip_count = 0;

char_t get_at(buffer_t *bp, point_t pt)
{
	return (*ptr(bp, pt));
}

void set_parse_state(buffer_t *bp, point_t pt)
{
	point_t p_saved = bp->b_point;
	point_t p_before1, p_before2, p_after, q_before, q_after;
	p_before1 = p_before2 = p_after = q_before = q_after = -1;

	p_before1 = (pt == 0 ? -1 : search_backwards(bp, pt, "/*"));
	p_before2 = (p_before1 == -1 ? -1 : search_backwards(bp, pt, "*/"));
	p_after =  (p_before1 == -1 ? -1 : search_forward(bp, pt, "*/"));
	bp->b_point = p_saved;
	skip_count = 0;
	state = next_state = ID_DEFAULT;

	/* first line on screen is potentially inside a block comment, which starts off screen */
	if (p_before1 > -1 && p_after > -1 && p_before1 <= pt && p_after >= pt && p_before2 < p_before1) {
		q_before = search_backwards(bp, p_before1, "\"");
		q_after = (q_before == -1 ? -1 : search_forward(bp, p_before1, "\""));
		bp->b_point = p_saved;
		/* check our sequence was NOT quote something start_comment something quote */
		if (q_before > -1 && q_after > -1 && q_before < p_before1 && q_after > p_before1)
			return;
		state = next_state = ID_BLOCK_COMMENT;
	}
}

int parse_text(buffer_t *bp, point_t pt)
{
	if (skip_count-- > 0)
		return state;

	char_t c_now = get_at(bp, pt);
	char_t c_next = get_at(bp, pt + 1);
	state = next_state;

	if (state == ID_DEFAULT && c_now == '/' && c_next == '*') {
		skip_count = 1;
		return (next_state = state = ID_BLOCK_COMMENT);
	}

	if (state == ID_BLOCK_COMMENT && c_now == '*' && c_next == '/') {
		skip_count = 1;
		next_state = ID_DEFAULT;
		return ID_BLOCK_COMMENT;
	}

	if (state == ID_DEFAULT && c_now == '/' && c_next == '/') {
		skip_count = 1;
		return (next_state = state = ID_LINE_COMMENT);
	}

	if (state == ID_LINE_COMMENT && c_now == '\n')
		return (next_state = ID_DEFAULT);

	if (state == ID_DEFAULT && c_now == '"')
		return (next_state = ID_DOUBLE_STRING);

	if (state == ID_DOUBLE_STRING && c_now == '\\') {
		skip_count = 1;
		return (next_state = ID_DOUBLE_STRING);
	}

	if (state == ID_DOUBLE_STRING && c_now == '"') {
		next_state = ID_DEFAULT;
		return ID_DOUBLE_STRING;
	}

	if (state == ID_DEFAULT && c_now == '\'')
		return (next_state = ID_SINGLE_STRING);

	if (state == ID_SINGLE_STRING && c_now == '\\') {
		skip_count = 1;
		return (next_state = ID_SINGLE_STRING);
	}

	if (state == ID_SINGLE_STRING && c_now == '\'') {
		next_state = ID_DEFAULT;
		return ID_SINGLE_STRING;
	}

	if (state != ID_DEFAULT)
		return (next_state = state);

	if (state == ID_DEFAULT && c_now >= '0' && c_now <= '9') {
		next_state = ID_DEFAULT;
		return (state = ID_DIGITS);
	}

	if (state == ID_DEFAULT && NULL != strchr("{}[]()!'£$%^&*-+=:;@~#<>,.?/\\|", c_now)) {
		next_state = ID_DEFAULT;
		return (state = ID_SYMBOL);
	}

	return (next_state = state);
}
