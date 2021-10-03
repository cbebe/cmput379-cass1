#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char *file = "./logger.util";

int main() {
  int fd[2];
  int od[2];
  char buf[] = "HELLO WORLD!";
  if (pipe(fd)) {
    perror("pipe");
    return -1;
  }
  if (pipe(od)) {
    perror("pipe");
    return -1;
  }
  switch (fork()) {
    case -1:
      perror("fork");
      return -1;
    case 0:
      // child
      close(fd[1]);
      dup2(fd[0], STDIN_FILENO);
      close(fd[0]);
      close(od[0]);
      dup2(od[1], STDOUT_FILENO);
      close(od[1]);
      char *args[] = {file, NULL};
      execvp(args[0], args);
      break;
    default: {
      FILE *output = fopen("output", "w");
      if (output == NULL) {
        perror("fopen");
        return -1;
      }

      // parent
      close(fd[0]);
      write(fd[1], buf, sizeof(buf));
      close(fd[1]);
      close(od[1]);
      char big_buf[64] = {};
      read(od[0], big_buf, sizeof(big_buf));
      fputs(big_buf, output);
      close(od[0]);
      wait(NULL);
      fclose(output);
    }
  }
  printf("END~\n");
  return 0;
}
