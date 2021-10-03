#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>

void print_usage(FILE *fp) {
  struct rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  fprintf(fp, "User time = \t %ld seconds\n", usage.ru_utime.tv_sec);
  fprintf(fp, "Sys  time = \t %ld seconds\n", usage.ru_stime.tv_sec);
}

int main(int argc, char const *argv[]) {
  time_t start = time(NULL);
  srand(start);

  int max_seconds = argc < 2 ? 10 : atoi(argv[1]);

  while ((time(NULL) - start) <= max_seconds) {
    for (int i = 0; i < 20; ++i) {
      FILE *fp = fopen("/dev/null", "w");
      fprintf(fp, "%d", i);
      fclose(fp);
    }
  }

  // FILE *fp = fopen("/dev/null", "w");
  // print_usage(fp);
  // fclose(fp);

  print_usage(stdout);

  return 0;
}
