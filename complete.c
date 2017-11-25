/* complete.c, Atto Emacs, Hugh Barney, Public Domain, 2016 */

#include "header.h"

/* basic filename completion, based on code in uemacs/PK */
int getfilename(char *prompt, char *buf, int nbuf)
{
	int cpos = 0;	/* current character position in string */
	int c, ocpos, n, nskip = 0, didtry = 0, iswild = 0, result = 0;

	char sys_command[255];
	char *output_file = NULL;
	FILE *fp = NULL;
	buf[0] ='\0';

	for (;;) {
		if (!didtry)
			nskip = -1;
		didtry = 0;
		display_prompt_and_response(prompt, buf);
		c = getch(); /* get a character from the user */

		switch(c) {
		case 0x0a: /* cr, lf */
		case 0x0d:
			buf[cpos] = 0;
			if (fp != NULL)
				fclose(fp);
			return (cpos > 0 ? TRUE : FALSE);

		case 0x07: /* ctrl-g, abort */
			if (fp != NULL)
				fclose(fp);
			return FALSE;

		case 0x7f: /* del, erase */
		case 0x08: /* backspace */
			if (cpos == 0)
				continue;
			buf[--cpos] = '\0';
			break;

		case  0x15: /* C-u kill */
			cpos = 0;
			buf[0] = '\0';
			break;

		case 0x09: /* TAB, complete file name */
			didtry = 1;
			ocpos = cpos;

			/* scan backwards for a wild card and set */
			iswild=0;
			while (cpos > -1) {
				if (buf[cpos] == '*' || buf[cpos] == '?')
					iswild = 1;
				cpos--;
			}
			cpos=0;

			/* first time retrieval */
			if (nskip < 0) {
				buf[ocpos] = 0;
				if (fp != NULL)
					fclose(fp);
				strcpy(sys_command, "echo ");
				strcat(sys_command, buf);
				if (!iswild)
					strcat(sys_command, "*");
				strcat(sys_command, " >");
				output_file = get_temp_file();
				strcat(sys_command, output_file);
				strcat(sys_command, " 2>&1");
				result = system(sys_command);
				result++; /* stop compiler warning about not used */
				fp = fopen(output_file, "r");
				nskip = 0;
			}

			/* skip to start of next filename in the list */
			c = ' ';
			for (n = nskip; n > 0; n--)
				while ((c = getc(fp)) != EOF && c != ' ')
					;
			nskip++;

			/* at end of list */
			if (c != ' ')
				nskip = 0;

			/* copy next filename into buf */
			while ((c = getc(fp)) != EOF && c != '\n' && c != ' ' && c != '*')
				if (cpos < nbuf - 1)
					buf[cpos++] = c;

			buf[cpos] = '\0';
			rewind(fp);
			unlink(output_file);
			break;

		default:
			if (cpos < nbuf - 1) {
				  buf[cpos++] = c;
				  buf[cpos] = '\0';
			}
			break;
		}
	}
}
