#include "main.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
get_input (input_options *options)
{
  char *line = NULL;
  size_t len = LINE_LENGTH;
  ssize_t line_size = getline (&line, &len, stdin);

  if (line_size > LINE_LENGTH)
    {
      fprintf (stderr, "Invalid line length\n");
      free (line);
      return;
    }

  char *arg;
  // don't want to ruin the original string
  char line_cpy[LINE_LENGTH];
  strncpy (line_cpy, line, LINE_LENGTH);
  arg = strtok (line_cpy, " ");
  while (arg != NULL)
    {
      size_t arg_length = strlen (arg);

      if (arg_length > MAX_LENGTH)
        {
          fprintf (stderr, "Invalid argument length\n");
          free (line);
          return;
        }

      if (options->argc > MAX_ARGS)
        {
          fprintf (stderr, "Invalid argument count\n");
          free (line);
          return;
        }

      // trim newlines
      arg[strcspn (arg, "\n")] = '\0';

      strncpy (options->argv[options->argc++], arg, MAX_LENGTH);
      arg = strtok (NULL, " ");
    }

  free (line); // getline allocates memory
}

int
get_int (input_options *options, int *integer)
{
  if (options->argc < 2)
    {
      return 0;
    }

  char *int_str = options->argv[1];
  if (strspn (int_str, "0123456789") != strlen (int_str))
    {
      return 0;
    }
  sscanf (int_str, "%d%*c", integer);
  return 1;
}