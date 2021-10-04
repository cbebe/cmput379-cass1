#include <signal.h>        // signal numbers
#include <stdio.h>         // fprintf, printf, stderr, popen
#include <stdlib.h>        // exit
#include <string.h>        // strtok
#include <sys/resource.h>  // getrusage
#include <sys/wait.h>      // waitpid, WNOHANG
#include <unistd.h>        // kill, perror

#include "main.h"

int get_cmd_options(struct parsed_input *input, struct cmd_options *cmd);
struct cmd_options *new_cmd_options();
void delete_cmd_options(struct cmd_options *options);

/**
 * Send signal to pid, perror and returns from function if it fails
 * This is probably the worst syntactic macro by far
 */
#define send_signal(pid, signal, message) \
  if (kill(pid, signal) != 0) {           \
    perror(#message " failed");           \
    return;                               \
  }

// forward declaration of process functions
void child_exec(struct cmd_options *);
void parent_exec(struct process_table *, struct cmd_options *, int);

int get_job(struct process_table *self, int pid) {
  for (int i = 0; i < self->num_processes; ++i) {
    if (self->processes[i].pid == pid) return i;
  }

  return -1;
}

/**
 * Prints the total processor usage of the shell's children
 */
void print_resource_usage() {
  struct rusage usage;
  getrusage(RUSAGE_CHILDREN, &usage);
  printf("User time = \t %ld seconds\n", usage.ru_utime.tv_sec);
  printf("Sys  time = \t %ld seconds\n", usage.ru_stime.tv_sec);
}

/**
 * Wait for all child processes and then exit
 */
void wait_and_exit() {
  int pid = 0;
  while ((pid = wait(NULL)) > 0)
    ;
  printf("Resources used\n");
  print_resource_usage();
  exit(0);
}

/**
 * Update process from process table if it's present in the ps output
 */
void process_ps_line(struct process_table *self, char *buf) {
  char *tok = strtok(buf, " ");  // PID
  pid_t pid = atoi(tok);
  // search process table for ps entry
  for (int j = 0; j < self->num_processes; ++j) {
    if (self->processes[j].pid == pid) {
      tok = strtok(NULL, " ");            // TIME token
      strtok(tok, ":");                   // HH, not needed
      strtok(NULL, ":");                  // MM, not needed
      char *seconds = strtok(NULL, ":");  // SS
      self->processes[j].time = atoi(seconds);
    }
  }
}

/**
 * Get output from `ps` command and process each line
 */
void get_ps_output(struct process_table *self) {
  // https://stackoverflow.com/questions/646241/c-run-a-system-command-and-get-output
  FILE *fp = popen("ps -o pid,time", "r");
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

void show_jobs(struct process_table *self) {
  printf("Running processes:\n");
  if (self->num_processes > 0) {
    get_ps_output(self);
    printf(" #      PID S SEC COMMAND\n");
    for (int i = 0; i < self->num_processes; ++i) {
      struct process *p = &self->processes[i];
      // Max PID in undergraduate machines:
      // cat /proc/sys/kernel/pid_max
      // 4194304
      printf("%2d: %7d %c%4d %s\n", i, p->pid, p->status, p->time, p->cmd);
    }
  }

  printf("Processes =\t %d active\n", self->num_processes);
  printf("Completed processes:\n");
  print_resource_usage();
}

void resume_job(struct process_table *self, int pid) {
  int idx = get_job(self, pid);
  if (idx < 0) throw("Process not found\n");
  struct process *p = &self->processes[idx];

  if (p->status == RUNNING) throw("Process already running\n");

  send_signal(pid, SIGCONT, Resume);

  p->status = RUNNING;
}

void wait_job(struct process_table *self, int pid) {
  int idx = get_job(self, pid);
  if (idx < 0) throw("Process not found\n");
  struct process *p = &self->processes[idx];

  // don't let program hang by waiting for a suspended process
  if (p->status == SUSPENDED) resume_job(self, pid);

  int stat_loc = 0;
  waitpid(p->pid, &stat_loc, 0);
}

void kill_job(struct process_table *self, int pid) {
  int idx = get_job(self, pid);
  if (idx < 0) throw("Process not found\n");

  send_signal(pid, SIGKILL, Kill);
}

void suspend_job(struct process_table *self, int pid) {
  int idx = get_job(self, pid);
  if (idx < 0) throw("Process not found\n");
  struct process *p = &self->processes[idx];

  if (p->status == SUSPENDED) throw("Process already suspended\n");

  send_signal(pid, SIGSTOP, Suspend);

  p->status = SUSPENDED;
}

void new_job(struct process_table *self, struct cmd_options *options) {
  if (options->bg && self->num_processes >= MAX_PT_ENTRIES)
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

void reap_children(struct process_table *self) {
  int status = 0, ongoing_children = 0;
  struct process temp[MAX_PT_ENTRIES];
  for (int i = 0; i < self->num_processes; ++i) {
    // process is still ongoing
    if (!waitpid(self->processes[i].pid, &status, WNOHANG))
      temp[ongoing_children++] = self->processes[i];
  }

  // copy back to array
  for (int i = 0; i < ongoing_children; ++i) self->processes[i] = temp[i];

  self->num_processes = ongoing_children;
}

void run_command(struct process_table *table, struct parsed_input *input) {
  struct cmd_options *cmd = new_cmd_options();
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
