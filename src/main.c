#include "main.h"

#include <signal.h>  // signal numbers
#include <stdio.h>   // printf, fprintf, stderr, fflush, stdout
#include <stdlib.h>  // exit
#include <unistd.h>  // sleep

// forward declarations

// process table functions
void show_jobs(struct process_table *);
void wait_job(struct process_table *, int);
void kill_job(struct process_table *, int);
void resume_job(struct process_table *, int);
void suspend_job(struct process_table *, int);
void reap_children(struct process_table *);
void run_command(struct process_table *, struct parsed_input *);
void wait_and_exit();

// parser functions
int get_input(struct parsed_input *);
int get_int(struct parsed_input *, int *);

/**
 * Bool macro for matching strings
 */
#define match(s1, s2) strncmp(s1, s2, MAX_LENGTH) == 0

/**
 * Does a table command after requiring a PID
 * This is how I start writing unreadable C code
 */
#define table_command(command, table, input)             \
  do {                                                   \
    int pid = 0;                                         \
    if (!get_int(&input, &pid)) throw("PID required\n"); \
    command(table, pid);                                 \
  } while (0)

// make table available to the sigchld_handler
struct process_table table = {.num_processes = 0};

void sigchld_handler() { reap_children(&table); }

/**
 * Wait for user input and execute
 */
void run(struct process_table *table) {
  struct parsed_input input = {.tokc = 0};
  if (!get_input(&input)) return;

  char *command = input.tokv[0];

  if (match(command, "exit"))
    wait_and_exit();
  else if (match(command, "jobs"))
    show_jobs(table);
  else if (match(command, "kill"))
    table_command(kill_job, table, input);
  else if (match(command, "resume"))
    table_command(resume_job, table, input);
  else if (match(command, "suspend"))
    table_command(suspend_job, table, input);
  else if (match(command, "wait"))
    table_command(wait_job, table, input);
  else if (match(command, "sleep")) {
    int seconds = 0;
    if (get_int(&input, &seconds)) {
      sleep(seconds);  // call sleep system call directly
    } else {
      run_command(table, &input);  // call UNIX sleep command
    }
  } else
    // not a built-in command, try to execute program
    run_command(table, &input);
}

int main() {
  // https://en.wikipedia.org/wiki/C_signal_handling
  if (signal(SIGCHLD, sigchld_handler) == SIG_ERR) {
    fprintf(stderr, "Error setting signal listener\n");
    exit(1);
  }
  while (1) {
    if (TERMINAL) {
      printf("shell379> ");  // print prompt
      fflush(stdout);
    }
    run(&table);
  }
}
