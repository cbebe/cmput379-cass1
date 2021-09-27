#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

void print_cmd_options (struct cmd_options *options);

/**
 * Returns from function if process pointer is null
 */
#define return_if_null(p)                                                     \
  if (p == NULL)                                                              \
    {                                                                         \
      throw ("Process not found\n");                                          \
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
show_jobs (struct process_table *self)
{
  if (self->num_processes > 0)
    {
      printf (" #    PID S SEC COMMAND\n");
      for (int i = 0; i < self->num_processes; ++i)
        {
          struct process *p = &self->processes[i];
          printf ("%2d: %5d %c%4d %s", i, p->pid, p->status, 0, p->cmd);
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

  if (kill (pid, SIGKILL) != 0)
    throw ("Kill failed");

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

  if (kill (pid, SIGCONT) != 0)
    throw ("Resume failed\n");

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

  if (kill (pid, SIGSTOP) != 0)
    throw ("Suspend failed\n");

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
      fprintf (stderr, "Error forking child process\n");
      exit (1);
    }
  else if (pid == 0)
    {
      // child process
      if (execvp (options->argv[0], options->argv) < 0)
        {
          fprintf (stderr, "execvp failed\n");
          exit (1);
        }
    }

  // parent process

  if (options->bg == BACKGROUND)
    {
      // process run in background, add to table
      struct process p = { .pid = pid, .status = RUNNING, .cmd = "" };
      printf ("%s\n", options->cmd);
      strncpy (p.cmd, options->cmd, LINE_LENGTH);
      printf ("%s\n", p.cmd);
      self->processes[self->num_processes++] = p;
    }
  else
    {
      int status = 0;
      waitpid (pid, &status, 0);
    }
}

// used for debugging
void
print_cmd_options (struct cmd_options *options)
{
  printf ("Command: %s\n", options->cmd);
  printf ("Args: ");
  for (int i = 0; i < options->argc; ++i)
    {
      printf ("%s ", options->argv[i]);
    }
  printf ("\nInput: ");
  for (int i = 0; i < options->ifc; ++i)
    {
      printf ("%s ", options->ifv[i]);
    }
  printf ("\nOutput: ");
  for (int i = 0; i < options->ofc; ++i)
    {
      printf ("%s ", options->ofv[i]);
    }
  printf ("\n");
}