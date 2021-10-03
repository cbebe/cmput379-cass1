#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>     // printf, fprintf, fileno, stdin, stderr
#include <string.h>    // strncmp, strcspn
#include <sys/wait.h>  // wait
#include <unistd.h>    // isatty

#define LINE_LENGTH 100    // Max # of characters in an input line
#define MAX_LENGTH 20      // Max # of characters in an argument
#define MAX_ARGS 7         // Max number of arguments to a command
#define MAX_PT_ENTRIES 32  // Max entries in the Process Table

#define remove_newline(line) line[strcspn(line, "\n")] = '\0'
#define TERMINAL isatty(fileno(stdin))

#define throw(message)        \
  do {                        \
    fprintf(stderr, message); \
    return;                   \
  } while (0)

enum process_status {
  RUNNING = 'R',    // process is running in background
  SUSPENDED = 'S',  // process in background is suspended
};

struct process {
  pid_t pid;
  int time;
  enum process_status status;
  char cmd[LINE_LENGTH];
};

struct process_table {
  int num_processes;
  struct process processes[MAX_PT_ENTRIES];
};

enum process_type {
  FOREGROUND = 0,
  BACKGROUND = 1,
};

// struct for storing options when running a command
struct cmd_options {
  char *cmd;  // the entire command

  int to_file;  // bool values
  int from_file;

  char *in_file;  // filenames
  char *out_file;

  int argc;              // argument count
  char *argv[MAX_ARGS];  // argument vector
  enum process_type bg;
};

struct parsed_input {
  char cmd[LINE_LENGTH];            // the entire command
  int tokc;                         // token count
  char tokv[MAX_ARGS][MAX_LENGTH];  // token vector
};

#endif  // MAIN_H_
