#include <stdio.h>

int
main ()
{
  char *buf = NULL;
  size_t len = 0;
  ssize_t lineSize = getline (&buf, &len, stdin);
  printf ("log: %s", buf);
}