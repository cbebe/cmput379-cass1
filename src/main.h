#ifndef SRC_MAIN
#define SRC_MAIN

#define LINE_LENGTH 100   // Max # of characters in an input line
#define MAX_LENGTH 20     // Max # of characters in an argument
#define MAX_ARGS 7        // Max number of arguments to a command
#define MAX_PT_ENTRIES 32 // Max entries in the Process Table

typedef struct
{
  int pid;
  char cmd[LINE_LENGTH];
} process;

typedef struct
{
  int num_processes;
  process processes[MAX_PT_ENTRIES];
} process_table;

typedef struct
{
  int argc;
  char argv[MAX_ARGS][MAX_LENGTH];
} input_options;

void hello ();
void get_input (input_options *options);

#endif /* SRC_MAIN */
