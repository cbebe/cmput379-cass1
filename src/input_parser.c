#include "main.h"
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

      strncpy (options->argv[options->argc++], arg, MAX_LENGTH);
      arg = strtok (NULL, " ");
    }

  for (int i = 0; i < options->argc; ++i)
    {
      if (options->argv[i][0] != '\n')
        {
          printf ("%s\n", options->argv[i]);
        }
    }

  free (line); // getline allocates memory
}
