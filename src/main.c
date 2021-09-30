#include "main.h"

#include <stdio.h>   // printf, fprintf, stderr, fflush, stdout
#include <stdlib.h>  // exit
#include <unistd.h>  // sleep

// process table functions
void show_jobs(struct process_table* self);
void wait_job(struct process_table* self, int pid);
void kill_job(struct process_table* self, int pid);
void resume_job(struct process_table* self, int pid);
void suspend_job(struct process_table* self, int pid);
void reap_children(struct process_table* self);
void run_command(struct process_table* table, struct parsed_input* input);

// parser functions
int get_input(struct parsed_input* options);
int get_int(struct parsed_input* options, int* integer);

// this is how i start writing unreadable c code
#define require_int(options, pid, message) \
  if (!get_int(options, pid)) {            \
    throw(message);                        \
  }

// please stop me
#define table_command(command, table, input)     \
  do {                                           \
    int pid = 0;                                 \
    require_int(&input, &pid, "PID required\n"); \
    command(table, pid);                         \
  } while (0)

struct process_table table = {.num_processes = 0, .processes = {}};

void sigchild_handler() {
  // https://stackoverflow.com/questions/3599160/how-to-suppress-unused-parameter-warnings-in-c/12891181
  // (void)(sig);
  reap_children(&table);
}

void run(struct process_table* table) {
  struct parsed_input input = {.argc = 0, .argv = {}};
  if (!get_input(&input)) return;

  char* command = input.argv[0];

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
    require_int(&input, &seconds, "Seconds required\n");
    sleep(seconds);
  } else
    run_command(table, &input);
}

int main() {
  // https://en.wikipedia.org/wiki/C_signal_handling
  if (signal(SIGCHLD, sigchild_handler) == SIG_ERR) {
    fprintf(stderr, "Error setting signal listener\n");
    exit(1);
  }
  while (1) {
    if (TERMINAL) {
      // print prompt
      printf("shell379> ");
      fflush(stdout);
    }
    run(&table);
  }
}
