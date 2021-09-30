#include <stdio.h>         // fprintf, printf, stderr, popen
#include <stdlib.h>        // exit
#include <string.h>        // strtok
#include <sys/resource.h>  // getrusage
#include <sys/wait.h>      // waitpid, WNOHANG
#include <unistd.h>        // kill, perror, and signal numbers

#include "main.h"

int get_cmd_options(struct parsed_input* input, struct cmd_options* cmd);
struct cmd_options* new_cmd_options();
void delete_cmd_options(struct cmd_options* options);

/**
 * Returns from function if process pointer is null
 */
#define return_if_null(p)         \
  if (p == NULL) {                \
    throw("Process not found\n"); \
  }

/**
 * Send signal to pid, perror and returns from function if it fails
 */
#define send_signal(pid, signal, message) \
  if (kill(pid, signal) != 0) {           \
    perror(#message " failed");           \
    return;                               \
  }

void child_exec(struct cmd_options* options);
void parent_exec(struct process_table* self, struct cmd_options* options,
                 int pid);

struct process* get_job(struct process_table* self, int pid, int* idx) {
  for (int i = 0; i < self->num_processes; ++i) {
    if (self->processes[i].pid == pid) {
      *idx = i;
      return &self->processes[i];
    }
  }

  return NULL;
}

void print_resource_usage() {
  struct rusage usage;
  getrusage(RUSAGE_CHILDREN, &usage);
  printf("User time = \t %ld seconds\n", usage.ru_utime.tv_sec);
  printf("Sys  time = \t %ld seconds\n", usage.ru_stime.tv_sec);
}

void process_ps_line(struct process_table* self, char* buf) {
  char* tok = strtok(buf, " ");  // PID
  pid_t pid = atoi(tok);
  strtok(NULL, " ");  // TTY
  // search process table for ps entry
  for (int j = 0; j < self->num_processes; ++j) {
    if (self->processes[j].pid == pid) {
      tok = strtok(NULL, " ");            // TIME
      strtok(tok, ":");                   // HH
      strtok(NULL, ":");                  // MM
      char* seconds = strtok(NULL, ":");  // SS
      self->processes[j].time = atoi(seconds);
    }
  }
}

void get_ps_output(struct process_table* self) {
  // https://stackoverflow.com/questions/646241/c-run-a-system-command-and-get-output
  FILE* fp = popen("ps", "r");
  if (fp == NULL) {
    printf("Failed to run command\n");
    exit(1);  // no ps no life
  }

  int skip_line = 1;
  char buf[64];
  while (fgets(buf, sizeof(buf), fp) != NULL) {
    if (skip_line == 1) {
      skip_line = 0;
      continue;
    }

    remove_newline(buf);
    process_ps_line(self, buf);
  }

  pclose(fp);
}

void show_jobs(struct process_table* self) {
  printf("Running processes:\n");
  if (self->num_processes > 0) {
    get_ps_output(self);
    printf(" #    PID S SEC COMMAND\n");
    for (int i = 0; i < self->num_processes; ++i) {
      struct process* p = &self->processes[i];
      printf("%2d: %5d %c%4d %s\n", i, p->pid, p->status, p->time, p->cmd);
    }
  }

  printf("Processes =\t %d active\n", self->num_processes);
  printf("Completed processes:\n");
  print_resource_usage();
}

void resume_job(struct process_table* self, int pid) {
  int idx = 0;
  struct process* p = get_job(self, pid, &idx);
  return_if_null(p);

  if (p->status == RUNNING) throw("Process already running\n");

  send_signal(pid, SIGCONT, Resume);

  p->status = RUNNING;
}

void remove_job(struct process_table* self, int idx) {
  // remove process from array
  for (int i = idx; i < self->num_processes - 1; ++i) {
    if (i + 1 < self->num_processes)
      self->processes[i] = self->processes[i + 1];
  }
  --self->num_processes;
}

void wait_job(struct process_table* self, int pid) {
  int idx = 0;
  struct process* p = get_job(self, pid, &idx);
  return_if_null(p);

  // don't let program hang by waiting for a suspended process
  if (p->status == SUSPENDED) resume_job(self, pid);

  int stat_loc = 0;
  waitpid(p->pid, &stat_loc, 0);
  remove_job(self, idx);
}

void kill_job(struct process_table* self, int pid) {
  int idx = 0;
  struct process* p = get_job(self, pid, &idx);
  return_if_null(p);

  send_signal(pid, SIGKILL, Kill);

  // it's a zombie process by this point, might as well reap
  wait_job(self, pid);
}

void suspend_job(struct process_table* self, int pid) {
  int idx = 0;
  struct process* p = get_job(self, pid, &idx);
  return_if_null(p);

  if (p->status == SUSPENDED) throw("Process already suspended\n");

  send_signal(pid, SIGSTOP, Suspend);

  p->status = SUSPENDED;
}

void new_job(struct process_table* self, struct cmd_options* options) {
  if (self->num_processes >= MAX_PT_ENTRIES)
    throw("Maximum number of processes reached\n");

  pid_t pid = fork();
  if (pid < 0) {
    perror("Error forking child process");
    exit(1);
  } else if (pid == 0)
    child_exec(options);

  // parent process
  parent_exec(self, options, pid);
}

void reap_children(struct process_table* self) {
  int status = 0;
  for (int i = 0; i < self->num_processes; ++i) {
    if (waitpid(self->processes[i].pid, &status, WNOHANG))
      remove_job(self, i);  // child died
  }
}

void run_command(struct process_table* table, struct parsed_input* input) {
  struct cmd_options* cmd = new_cmd_options();
  if (!get_cmd_options(input, cmd)) {
    fprintf(stderr, "Invalid input\n");
    delete_cmd_options(cmd);
    return;
  }

  if (cmd->argc == 0) {
    fprintf(stderr, "No command given\n");
    delete_cmd_options(cmd);
    return;
  }

  new_job(table, cmd);
  delete_cmd_options(cmd);
}