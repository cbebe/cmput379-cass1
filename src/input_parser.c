#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "main.h"

#define input_error(line, message) \
  do {                             \
    fprintf(stderr, message);      \
    free(line);                    \
    return 0;                      \
  } while (0)

/**
 * Split input by whitespace and verify validity
 * Return 1 for valid input, 0 for invalid
 */
int split_input(struct parsed_input* options, char* line) {
  char* arg;
  // don't want to ruin the original string
  char line_cpy[LINE_LENGTH];
  strncpy(line_cpy, line, LINE_LENGTH);
  arg = strtok(line_cpy, " ");
  while (arg != NULL) {
    size_t arg_length = strlen(arg);

    if (arg_length > MAX_LENGTH) input_error(line, "Invalid argument length\n");

    if (options->argc > MAX_ARGS - 1)
      input_error(line, "Invalid argument count\n");

    remove_newline(arg);

    strncpy(options->argv[options->argc++], arg, MAX_LENGTH);
    arg = strtok(NULL, " ");
  }

  return 1;
}

int get_input(struct parsed_input* options) {
  char* line = NULL;
  size_t len = LINE_LENGTH;
  ssize_t line_size = getline(&line, &len, stdin);

  // EOF, done
  if (line_size < 0) {
    if (TERMINAL) printf("\n");
    wait_and_exit();
  }

  // only the newline is present
  if (line_size == 1) return 0;

  if (line_size > LINE_LENGTH) input_error(line, "Invalid line length\n");

  remove_newline(line);

  if (!split_input(options, line)) return 0;

  strncpy(options->cmd, line, LINE_LENGTH);
  free(line);  // getline allocates memory
  return 1;
}

// get an integer from the argument vector
// return 1 if it's a valid int, 0 if not
int get_int(struct parsed_input* options, int* integer) {
  if (options->argc != 2) return 0;

  char* int_str = options->argv[1];
  if (strspn(int_str, "0123456789") != strlen(int_str)) return 0;

  sscanf(int_str, "%d%*c", integer);
  return 1;
}

// allocate memory for a command options struct
struct cmd_options* new_cmd_options() {
  struct cmd_options* options =
      (struct cmd_options*)malloc(sizeof(struct cmd_options));

  // init values
  options->argc = 0, options->to_file = 0, options->from_file = 0;
  options->bg = FOREGROUND;

  options->cmd = (char*)malloc(sizeof(char) * LINE_LENGTH);
  for (int i = 0; i < MAX_ARGS; ++i)
    options->argv[i] = (char*)malloc(sizeof(char) * MAX_LENGTH);

  options->in_file = (char*)malloc(sizeof(char) * MAX_LENGTH);
  options->out_file = (char*)malloc(sizeof(char) * MAX_LENGTH);

  return options;
}

/**
 * Look for special arguments in input by checking the tokens' first character
 * Returns 1 for valid input, 0 for invalid
 */
int check_special_args(struct parsed_input* input, struct cmd_options* cmd,
                       int i) {
  switch (input->argv[i][0]) {
    case '&':
      // makes sure that there's a command before '&'
      if (strlen(input->argv[i]) != 1 || input->argc == 1 ||
          i != input->argc - 1)
        return 0;
      cmd->bg = BACKGROUND;
      break;
    case '<':
      // makes sure that there's only one input file
      if (cmd->from_file) return 0;
      cmd->from_file = 1;
      strncpy(cmd->in_file, &input->argv[i][1], MAX_LENGTH);
      break;
    case '>':
      // makes sure that there's only one output file
      if (cmd->to_file) return 0;
      cmd->to_file = 1;
      strncpy(cmd->out_file, &input->argv[i][1], MAX_LENGTH);
      break;
    default:
      strncpy(cmd->argv[cmd->argc++], input->argv[i], MAX_LENGTH);
      break;
  }

  return 1;
}

/**
 * Get special command options from input array
 * Returns 1 on valid command input, 0 for invalid
 */
int get_cmd_options(struct parsed_input* input, struct cmd_options* cmd) {
  strncpy(cmd->cmd, input->cmd, LINE_LENGTH);

  for (int i = 0; i < input->argc; ++i) {
    if (!check_special_args(input, cmd, i)) return 0;
  }

  // last element in argv array must be null for execvp
  cmd->argv[cmd->argc] = (char*)NULL;
  return 1;
}

// frees memory allocated to a cmd_options struct
void delete_cmd_options(struct cmd_options* options) {
  for (int i = 0; i < MAX_ARGS; ++i) free(options->argv[i]);

  free(options->cmd);
  free(options->in_file);
  free(options->out_file);
  free(options);
}
