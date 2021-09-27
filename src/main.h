#ifndef SRC_MAIN
#define SRC_MAIN

#define LINE_LENGTH 100   // Max # of characters in an input line
#define MAX_LENGTH 20     // Max # of characters in an argument
#define MAX_ARGS 7        // Max number of arguments to a command
#define MAX_PT_ENTRIES 32 // Max entries in the Process Table

#include <string.h>

#define remove_newline(line) line[strcspn (line, "\n")] = '\0'
#define TERMINAL isatty (fileno (stdin))

#define print_usage_and_exit()                                                \
  do                                                                          \
    {                                                                         \
      printf ("Resources used\n");                                            \
      print_resource_usage ();                                                \
      exit (0);                                                               \
    }                                                                         \
  while (0)

#define throw(message)                                                        \
  do                                                                          \
    {                                                                         \
      fprintf (stderr, message);                                              \
      return;                                                                 \
    }                                                                         \
  while (0)

enum process_status
{
  // process is running in background
  RUNNING = 'R',
  // process in background is suspended
  SUSPENDED = 'S',
};

struct process
{
  int pid;
  int time;
  enum process_status status;
  char cmd[LINE_LENGTH];
};

struct process_table
{
  int num_processes;
  struct process processes[MAX_PT_ENTRIES];
};

enum process_type
{
  FOREGROUND = 0,
  BACKGROUND = 1,
};

struct cmd_options
{
  char *cmd;            // the entire command
  int ifc;              // input file count
  char *ifv[MAX_ARGS];  // input file vector
  int ofc;              // output file count
  char *ofv[MAX_ARGS];  // output file vector
  int argc;             // argument count
  char *argv[MAX_ARGS]; // argument vector
  enum process_type bg;
};

struct input_options
{
  char cmd[LINE_LENGTH];
  int argc;
  char argv[MAX_ARGS][MAX_LENGTH];
};

// process table functions
void show_jobs (struct process_table *self);
void kill_job (struct process_table *self, int pid);
void resume_job (struct process_table *self, int pid);
void suspend_job (struct process_table *self, int pid);
void wait_job (struct process_table *self, int pid);
void new_job (struct process_table *self, struct cmd_options *options);
void print_resource_usage ();

// parser functions
int get_input (struct input_options *options);
int get_cmd_options (struct input_options *input, struct cmd_options *cmd);
int get_int (struct input_options *options, int *integer);
struct cmd_options *new_cmd_options ();
void delete_cmd_options (struct cmd_options *options);
void print_cmd_options (struct cmd_options *options);

#endif /* SRC_MAIN */
