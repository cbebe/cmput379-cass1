#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "main.h"

// https://stackoverflow.com/questions/2605130/redirecting-exec-output-to-a-buffer-or-file
void redirect_stdout(const char* file) {
  int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  dup2(fd, STDOUT_FILENO);
  close(fd);
}

void redirect_stdin(const char* file) {
  int fd = open(file, O_RDONLY, S_IRUSR, STDIN_FILENO);
  dup2(fd, STDIN_FILENO);
  close(fd);
}

void child_exec(struct cmd_options* options) {
  if (options->to_file) redirect_stdout(options->out_file);
  if (options->from_file) redirect_stdin(options->in_file);

  // child process
  if (execvp(options->argv[0], options->argv) < 0) {
    perror("execvp failed");
    // https://stackoverflow.com/questions/5422831/what-is-the-difference-between-using-exit-exit-in-a-conventional-linux-fo
    _exit(1);
  }
}

void parent_exec(struct process_table* self, struct cmd_options* options,
                 int pid) {
  int status = 0;
  if (options->bg == BACKGROUND) {
    // process run in background, add to table
    struct process* p = &self->processes[self->num_processes++];
    p->pid = pid;
    p->status = RUNNING;
    p->time = 0;
    strncpy(p->cmd, options->cmd, LINE_LENGTH);
  } else
    // wait for process, don't add to table
    waitpid(pid, &status, 0);
}
