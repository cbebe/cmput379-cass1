#include "main.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define input_error(line, message)                                            \
  do                                                                          \
    {                                                                         \
      fprintf (stderr, message);                                              \
      free (line);                                                            \
      return 0;                                                               \
    }                                                                         \
  while (0)

int
get_input (struct input_options *options)
{
  char *line = NULL;
  size_t len = LINE_LENGTH;
  ssize_t line_size = getline (&line, &len, stdin);

  // only the newline is present
  if (line_size == 1)
    return 0;

  if (line_size > LINE_LENGTH)
    input_error (line, "Invalid line length\n");

  char *arg;
  // don't want to ruin the original string
  char line_cpy[LINE_LENGTH];
  strncpy (line_cpy, line, LINE_LENGTH);
  arg = strtok (line_cpy, " ");
  while (arg != NULL)
    {
      size_t arg_length = strlen (arg);

      if (arg_length > MAX_LENGTH)
        input_error (line, "Invalid argument length\n");

      if (options->argc > MAX_ARGS - 1)
        input_error (line, "Invalid argument count\n");

      // trim newlines
      arg[strcspn (arg, "\n")] = '\0';

      strncpy (options->argv[options->argc++], arg, MAX_LENGTH);
      arg = strtok (NULL, " ");
    }

  strncpy (options->cmd, line, LINE_LENGTH);
  free (line); // getline allocates memory
  return 1;
}

int
get_int (struct input_options *options, int *integer)
{
  if (options->argc < 2)
    return 0;

  char *int_str = options->argv[1];
  if (strspn (int_str, "0123456789") != strlen (int_str))
    return 0;

  sscanf (int_str, "%d%*c", integer);
  return 1;
}

struct cmd_options *
new_cmd_options ()
{
  struct cmd_options *options
      = (struct cmd_options *)malloc (sizeof (struct cmd_options));

  // init values
  options->argc = 0;
  options->ifc = 0;
  options->ofc = 0;
  options->bg = FOREGROUND;

  options->cmd = (char *)malloc (sizeof (char) * LINE_LENGTH);
  for (int i = 0; i < MAX_ARGS; ++i)
    {
      options->argv[i] = (char *)malloc (sizeof (char) * MAX_LENGTH);
      options->ifv[i] = (char *)malloc (sizeof (char) * MAX_LENGTH);
      options->ofv[i] = (char *)malloc (sizeof (char) * MAX_LENGTH);
    }

  return options;
}

int
get_cmd_options (struct input_options *input, struct cmd_options *cmd)
{
  strncpy (cmd->cmd, input->cmd, LINE_LENGTH);

  for (int i = 0; i < input->argc; ++i)
    {
      switch (input->argv[i][0])
        {
        case '&':
          // makes sure that there's a command before '&'
          if (strlen (input->argv[i]) != 1 || input->argc == 1
              || i != input->argc - 1)
            return 0;

          cmd->bg = BACKGROUND;
          break;
        case '<':
          strncpy (cmd->ifv[cmd->ifc++], &input->argv[i][1], MAX_LENGTH);
          break;
        case '>':
          strncpy (cmd->ofv[cmd->ofc++], &input->argv[i][1], MAX_LENGTH);
          break;
        default:
          strncpy (cmd->argv[cmd->argc++], input->argv[i], MAX_LENGTH);
          break;
        }
    }

  cmd->argv[cmd->argc] = (char *)NULL;
  return 1;
}

void
delete_cmd_options (struct cmd_options *options)
{
  for (int i = 0; i < MAX_ARGS; ++i)
    {
      free (options->argv[i]);
      free (options->ifv[i]);
      free (options->ofv[i]);
    }

  free (options->cmd);
  free (options);
}

// used for debugging
void
print_cmd_options (struct cmd_options *options)
{
  printf ("Command: %s\n", options->cmd);
  printf ("Args: ");
  for (int i = 0; i < options->argc; ++i)
    {
      printf ("%s ", options->argv[i]);
    }
  printf ("\nInput: ");
  for (int i = 0; i < options->ifc; ++i)
    {
      printf ("%s ", options->ifv[i]);
    }
  printf ("\nOutput: ");
  for (int i = 0; i < options->ofc; ++i)
    {
      printf ("%s ", options->ofv[i]);
    }
  printf ("\n");
}