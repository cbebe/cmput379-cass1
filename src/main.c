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
      throw (message);                                                        \
    }

void
run (struct process_table *table)
{
  // print prompt
  printf ("shell379> ");
  fflush (stdout);
  struct input_options options = { .argc = 0, .argv = {} };
  if (!get_input (&options))
    {
      return;
    }

  char *command = options.argv[0];

  if (match (command, "exit"))
    {
      printf ("Resources used\n");
      print_resource_usage ();
      exit (0);
    }
  else if (match (command, "jobs"))
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
      struct cmd_options *cmd = new_cmd_options ();
      if (!get_cmd_options (&options, cmd))
        {
          fprintf (stderr, "Invalid input of '&'\n");
          delete_cmd_options (cmd);
          return;
        }

      if (cmd->argc == 0)
        {
          fprintf (stderr, "No command given\n");
          delete_cmd_options (cmd);
          return;
        }

      new_job (table, cmd);
      delete_cmd_options (cmd);
    }
}

int
main ()
{
  struct process_table table = { .num_processes = 0, .processes = {} };
  while (1)
    {
      run (&table);
    }
}
