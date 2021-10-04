#include <fcntl.h>     // open and its mask bits
#include <stdio.h>     // perror
#include <string.h>    // strncpy
#include <sys/stat.h>  // S_IRUSR, S_IWUSR
#include <sys/wait.h>  // _wait, waitpid
#include <unistd.h>    // execvp, _exit, close, dup2 and file numbers

#include "main.h"

#define WRITE_BITMASK O_WRONLY | O_CREAT | O_TRUNC
#define READ_BITMASK O_RDONLY

/**
 * Redirect IO to a file
 */
int redirect(const char *file, int permissions, int stat, int fileno) {
  int fd = open(file, permissions, stat);
  if (fd < 0) {
    perror("opening file");
    return 0;
  }
  dup2(fd, fileno);
  close(fd);
  return 1;
}

void child_exec(struct cmd_options *options) {
  if (options->to_file) {
    // give everyone read and write permissions on the created file
    if (!redirect(options->out_file, WRITE_BITMASK, 0666, STDOUT_FILENO))
      _exit(1);
  }
  if (options->from_file) {
    if (!redirect(options->in_file, READ_BITMASK, 0664, STDIN_FILENO)) _exit(1);
  }

  // child process
  if (execvp(options->argv[0], options->argv) < 0) {
    perror("execvp failed");
    // difference between _exit and exit
    // https://stackoverflow.com/questions/5422831
    _exit(1);
  }
}

void parent_exec(struct process_table *table, struct cmd_options *options,
                 int pid) {
  int status = 0;
  if (options->bg == BACKGROUND) {
    // process run in background, add to table
    struct process *p = &table->processes[table->num_processes++];
    p->pid = pid;
    p->status = RUNNING;
    p->time = 0;
    strncpy(p->cmd, options->cmd, LINE_LENGTH);
  } else
    // wait for process, don't add to table
    waitpid(pid, &status, 0);
}
