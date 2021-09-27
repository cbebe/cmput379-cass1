#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void
print_usage (const char *name)
{
  fprintf (stderr, "USAGE: %s SECONDS\n", name);
  exit (1);
}

int
main (int argc, char const *argv[])
{
  if (argc < 2)
    {
      print_usage (argv[0]);
    }

  sleep (atoi (argv[1]));

  return 0;
}
