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
  char cmd[LINE_LENGTH];
  int argc;
  char argv[MAX_ARGS][MAX_LENGTH];
} input_options;

// process table functions
void show_jobs (process_table *self);
void kill_job (process_table *self, int pid);
void resume_job (process_table *self, int pid);
void suspend_job (process_table *self, int pid);
void wait_job (process_table *self, int pid);
void new_job (process_table *self, input_options *options);

// parser functions
void get_input (input_options *options);
int get_int (input_options *options, int *integer);

#endif /* SRC_MAIN */
