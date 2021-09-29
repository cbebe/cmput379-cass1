#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "main.h"

// this is how i start writing unreadable c code
#define require_int(options, pid, message)                                    \
  if (!get_int (options, pid))                                                \
    {                                                                         \
      throw (message);                                                        \
    }

// please stop me
#define table_command(command, table, input)                                  \
  do                                                                          \
    {                                                                         \
      int pid = 0;                                                            \
      require_int (&input, &pid, "PID required\n");                           \
      command (table, pid);                                                   \
    }                                                                         \
  while (0)

struct process_table table = { .num_processes = 0, .processes = {} };

void
sigchild_handler (int sig)
{
  // https://stackoverflow.com/questions/3599160/how-to-suppress-unused-parameter-warnings-in-c/12891181
  (void)(sig);
  reap_zombies (&table);
}

void
run (struct process_table *table)
{
  if (TERMINAL)
    {
      // print prompt
      printf ("shell379> ");
      fflush (stdout);
    }
  struct parsed_input input = { .argc = 0, .argv = {} };
  if (!get_input (&input))
    {
      return;
    }

  char *command = input.argv[0];

  if (match (command, "exit"))
    wait_and_exit ();
  else if (match (command, "jobs"))
    show_jobs (table);
  else if (match (command, "kill"))
    table_command (kill_job, table, input);
  else if (match (command, "resume"))
    table_command (resume_job, table, input);
  else if (match (command, "suspend"))
    table_command (suspend_job, table, input);
  else if (match (command, "wait"))
    table_command (wait_job, table, input);
  else if (match (command, "sleep"))
    {
      int seconds = 0;
      require_int (&input, &seconds, "Seconds required\n");
      sleep (seconds);
    }
  else
    {
      struct cmd_options *cmd = new_cmd_options ();
      if (!get_cmd_options (&input, cmd))
        {
          fprintf (stderr, "Invalid input\n");
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
  // https://en.wikipedia.org/wiki/C_signal_handling
  if (signal (SIGCHLD, sigchild_handler) == SIG_ERR)
    {
      fprintf (stderr, "Error setting signal listener\n");
      exit (1);
    }
  while (1)
    {
      run (&table);
    }
}
