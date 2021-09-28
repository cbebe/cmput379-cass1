#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
main (int argc, char *argv[])
{
  int fd[2];
  int od[2];
  char buf[] = "HELLO WORLD!";
  if (pipe (fd))
    {
      perror ("pipe");
      return -1;
    }
  switch (fork ())
    {
    case -1:
      perror ("fork");
      return -1;
    case 0:
      // child
      close (fd[1]);
      dup2 (fd[0], STDIN_FILENO);
      close (fd[0]);
      execl ("./logger", NULL);
    default:
      // parent
      close (fd[0]);
      write (fd[1], buf, sizeof (buf));
      close (fd[1]);
      wait (NULL);
    }
  printf ("END~\n");
  return 0;
}