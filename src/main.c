#include <stdio.h>
#include <stdlib.h>

#include "main.h"

void
run ()
{
  // print prompt
  printf ("shell379> ");
  fflush (stdout);
  input_options i = { .argc = 0, .argv = {} };

  get_input (&i);
}

int
main ()
{
  while (1)
    {
      run ();
    }
}
