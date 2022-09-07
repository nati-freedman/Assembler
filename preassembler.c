#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"
#include "macros.h"
#include "preassembler.h"

/* Executes pre-assembler algorithm. */
void preassembler_algo(char *arg)
{
	FILE *as_file = NULL, *am_file = NULL;
	char *line = NULL;
	char *first_field = NULL; /* First non-white space field of the line */
	struct macro *node = NULL;
	char *name = NULL, *defn = NULL; /* Macro attributes */
	char *am_filename = NULL;
	struct macros_table *m_table = (struct macros_table *) malloc(sizeof(struct macros_table *));
	char *as_filename = (char *) malloc(strlen(arg)+4);

	as_filename = strcat(strcpy(as_filename, arg), ".as");
	as_file = fopen(as_filename, "r");
	free(as_filename);

	am_filename = (char *) malloc(strlen(arg)+4);
	am_filename = strcat(strcpy(am_filename, arg), ".am");
	am_file = fopen(am_filename, "w");
	free(am_filename);

	/* Creating a new macros table */
	create_macros_table(m_table);

	line = (char *) malloc(MAX_LINE);
	first_field = (char *) malloc(MAX_LINE);

	/* Reading the next line from the as file */
	while (fgets(line, MAX_LINE, as_file))
	{
		/* Checking for an empty line */
		if (!(sscanf(line, "%s", first_field)))
		{
			fputs(line, am_file);
			continue;
		}

		if ((node = macro_lookup(first_field, m_table))) /* Checking for an existing macro */
		{
			fputs(node->defn, am_file);
		}
		else if (!strcmp(first_field, "macro")) /* Checking for a new macro definition */
		{
			name = (char *) malloc(MAX_MACRO_NAME);
			defn = (char *) calloc(1, sizeof(char));

			sscanf(line, "%*s %s", name); /* Storing the macro name */

			/* Storing the macro definition */
			while (fgets(line, MAX_LINE, as_file) && sscanf(line, "%s", first_field) && strcmp(first_field, "endmacro"))
			{
				defn = (char *) realloc(defn, strlen(defn)+strlen(line)+1);
				strcat(defn, line);
			}

			macro_install(name, defn, m_table); /* Installing the new macro in the macros table */
			free(name);
			free(defn);
		}
		else /* Non macro related line */
		{
			fputs(line, am_file);
		}
	}

	free_macros_table(m_table);
	free(line);
	free(first_field);
	fclose(as_file);
	fclose(am_file);
}
