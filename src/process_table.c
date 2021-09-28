#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Returns from function if process pointer is null
 */
#define return_if_null(p)                                                     \
  if (p == NULL)                                                              \
    {                                                                         \
      throw ("Process not found\n");                                          \
    }

/**
 * Send signal to pid, perror and returns from function if it fails
 */
#define send_signal(pid, signal, message)                                     \
  if (kill (pid, signal) != 0)                                                \
    {                                                                         \
      perror (#message " failed");                                            \
      return;                                                                 \
    }

struct process *
get_job (struct process_table *self, int pid, int *idx)
{
  for (int i = 0; i < self->num_processes; ++i)
    {
      if (self->processes[i].pid == pid)
        {
          *idx = i;
          return &self->processes[i];
        }
    }

  return NULL;
}

void
print_resource_usage ()
{
  struct rusage usage;
  getrusage (RUSAGE_CHILDREN, &usage);
  printf ("User time = \t %ld seconds\n", usage.ru_utime.tv_sec);
  printf ("Sys  time = \t %ld seconds\n", usage.ru_stime.tv_sec);
}

void
get_ps_output (struct process_table *self)
{
  // https://stackoverflow.com/questions/646241/c-run-a-system-command-and-get-output
  FILE *fp = popen ("ps", "r");
  if (fp == NULL)
    {
      printf ("Failed to run command\n");
      exit (1); // no ps no life
    }

  int i = 0;
  char buf[64];
  while (fgets (buf, sizeof (buf), fp) != NULL)
    {
      // skip first line
      if (i == 0)
        {
          i = 1;
          continue;
        }

      remove_newline (buf);

      char *tok = strtok (buf, " "); // PID
      pid_t pid = atoi (tok);
      strtok (NULL, " "); // TTY
      // search process table for ps entry
      for (int j = 0; j < self->num_processes; ++j)
        {
          if (self->processes[j].pid == pid)
            {
              tok = strtok (NULL, " ");           // TIME
              strtok (tok, ":");                  // HH
              strtok (NULL, ":");                 // MM
              char *seconds = strtok (NULL, ":"); // SS
              self->processes[j].time = atoi (seconds);
            }
        }
    }

  pclose (fp);
}

void
show_jobs (struct process_table *self)
{
  if (self->num_processes > 0)
    {
      get_ps_output (self);
      printf (" #    PID S SEC COMMAND\n");
      for (int i = 0; i < self->num_processes; ++i)
        {
          struct process *p = &self->processes[i];
          printf ("%2d: %5d %c%4d %s\n", i, p->pid, p->status, p->time,
                  p->cmd);
        }
    }

  printf ("Processes =\t %d active\n", self->num_processes);
  printf ("Completed processes:\n");
  print_resource_usage ();
}

void
kill_job (struct process_table *self, int pid)
{
  int idx = 0;
  struct process *p = get_job (self, pid, &idx);
  return_if_null (p);

  send_signal (pid, SIGKILL, Kill);

  // it's a zombie process by this point, might as well reap
  wait_job (self, pid);
}

void
resume_job (struct process_table *self, int pid)
{
  int idx = 0;
  struct process *p = get_job (self, pid, &idx);
  return_if_null (p);

  if (p->status == RUNNING)
    throw ("Process already running\n");

  send_signal (pid, SIGCONT, Resume);

  p->status = RUNNING;
}

void
suspend_job (struct process_table *self, int pid)
{
  int idx = 0;
  struct process *p = get_job (self, pid, &idx);
  return_if_null (p);

  if (p->status == SUSPENDED)
    throw ("Process already suspended\n");

  send_signal (pid, SIGSTOP, Suspend);

  p->status = SUSPENDED;
}

void
wait_job (struct process_table *self, int pid)
{
  int idx = 0;
  struct process *p = get_job (self, pid, &idx);
  return_if_null (p);

  // don't let program hang by waiting for a suspended process
  if (p->status == SUSPENDED)
    resume_job (self, pid);

  int stat_loc = 0;
  waitpid (p->pid, &stat_loc, 0);
  // remove process from array
  --self->num_processes;
  for (int i = idx; i < self->num_processes; ++i)
    {
      if (i + 1 < self->num_processes)
        self->processes[i] = self->processes[i + 1];
    }
}

void
new_job (struct process_table *self, struct cmd_options *options)
{
  if (self->num_processes >= MAX_PT_ENTRIES)
    throw ("Maximum number of processes reached\n");

  pid_t pid = fork ();
  if (pid < 0)
    {
      perror ("Error forking child process");
      exit (1);
    }
  else if (pid == 0)
    {
      // child process
      if (execvp (options->argv[0], options->argv) < 0)
        {
          perror ("execvp failed");
          exit (1);
        }
    }

  // parent process

  if (options->bg == BACKGROUND)
    {
      // process run in background, add to table
      struct process *p = &self->processes[self->num_processes++];
      p->pid = pid;
      p->status = RUNNING;
      p->time = 0;
      strncpy (p->cmd, options->cmd, LINE_LENGTH);
    }
  else
    {
      // wait for process, don't add to table
      int status = 0;
      waitpid (pid, &status, 0);
    }
}
