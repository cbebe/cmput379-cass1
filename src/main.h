#ifndef SRC_MAIN
#define SRC_MAIN

#include <stdio.h>     // printf, fprintf, fileno, stdin, stderr
#include <string.h>    // strncmp, strcspn
#include <sys/wait.h>  // wait
#include <unistd.h>    // isatty

#define LINE_LENGTH 100    // Max # of characters in an input line
#define MAX_LENGTH 20      // Max # of characters in an argument
#define MAX_ARGS 7         // Max number of arguments to a command
#define MAX_PT_ENTRIES 32  // Max entries in the Process Table

#define match(s1, s2) strncmp(s1, s2, MAX_LENGTH) == 0
#define remove_newline(line) line[strcspn(line, "\n")] = '\0'
#define TERMINAL isatty(fileno(stdin))

void print_resource_usage();

#define wait_and_exit()         \
  do {                          \
    int status = 0;             \
    wait(&status);              \
    printf("Resources used\n"); \
    print_resource_usage();     \
    exit(0);                    \
  } while (0)

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
  int pid;
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
  char* cmd;  // the entire command

  int to_file;  // bool values
  int from_file;

  char* in_file;  // filenames
  char* out_file;

  int argc;              // argument count
  char* argv[MAX_ARGS];  // argument vector
  enum process_type bg;
};

struct parsed_input {
  char cmd[LINE_LENGTH];
  int argc;
  char argv[MAX_ARGS][MAX_LENGTH];
};

#endif /* SRC_MAIN */
