#include <stdio.h>   // printf, scanf
#include <stdlib.h>  // atoll
#include <time.h>    // time, time_t

int long_long_job(long long max_seconds, long long num) {
  time_t start = time(NULL);

  int k = 0;
  for (long long i = 0; i < num; ++i) {
    for (long long j = 0; j < num; ++j) {
      k += 20;
      time_t diff = (time(NULL) - start);
      if (diff > max_seconds) {
        return k;
      }
    }
  }

  return k;
}

int main(int argc, char const *argv[]) {
  long long max_seconds = 0;
  long long num = 0;
  if (argc == 3) {
    max_seconds = atoll(argv[1]);
    num = atoll(argv[2]);
  } else if (argc == 2) {
    max_seconds = atoll(argv[1]);
    num = 200000000000;
  } else {
    scanf("%lld", &max_seconds);
    num = 200000000000;
  }
  printf("%lld", max_seconds);
  int result = long_long_job(max_seconds, num);
  printf("%d\n", result);

  return 0;
}
