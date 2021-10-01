#include <stdio.h>
#include <sys/types.h>  // ssize_t

int main() {
  char *buf = NULL;
  size_t len = 0;
  ssize_t lineSize = getline(&buf, &len, stdin);
  printf("log: %s, lineSize = %ld", buf, lineSize);
}
