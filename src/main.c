#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "main.h"

#define match(cmd, jobs) strncmp (cmd, jobs, MAX_LENGTH) == 0

// this is how i start writing unreadable c code
#define require_int(options, pid, message)                                    \
  if (!get_int (options, pid))                                                \
    {                                                                         \
      fprintf (stderr, message);                                              \
      return;                                                                 \
    }

void
run (process_table *table)
{
  // print prompt
  printf ("shell379> ");
  fflush (stdout);
  input_options options = { .argc = 0, .argv = {} };
  get_input (&options);

  char *command = options.argv[0];

  if (match (command, "jobs"))
    {
      show_jobs (table);
    }
  else if (match (command, "kill"))
    {
      int pid = 0;
      require_int (&options, &pid, "PID required\n");
      kill_job (table, pid);
    }
  else if (match (command, "resume"))
    {
      int pid = 0;
      require_int (&options, &pid, "PID required\n");
      resume_job (table, pid);
    }
  else if (match (command, "sleep"))
    {
      int seconds = 0;
      require_int (&options, &seconds, "Seconds required\n");
      sleep (seconds);
    }
  else if (match (command, "suspend"))
    {
      int pid = 0;
      require_int (&options, &pid, "PID required\n");
      suspend_job (table, pid);
    }
  else if (match (command, "wait"))
    {
      int pid = 0;
      require_int (&options, &pid, "PID required\n");
      wait_job (table, pid);
    }
  else
    {
      new_job (table, &options);
    }
}

int
main ()
{
  process_table table = { .num_processes = 0, .processes = {} };
  while (1)
    {
      run (&table);
    }
}
