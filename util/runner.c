// C program for implementation of Bubble sort
// https://www.geeksforgeeks.org/bubble-sort/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(int *xp, int *yp) {
  int temp = *xp;
  *xp = *yp;
  *yp = temp;
}

/* Function to print an array */
void print_array(int arr[], long long int size) {
  for (long long int i = 0; i < size; i++) printf("%d ", arr[i]);
  printf("\n");
}

// A function to implement bubble sort
void bubble_sort(int arr[], long long int n, long long int max_seconds) {
  time_t start = time(NULL);
  for (long long int i = 0; i < n - 1; i++) {
    // Last i elements are already in place
    for (long long int j = 0; j < n - i - 1; j++) {
      time_t diff = (time(NULL) - start);
      if (diff > max_seconds) {
        free(arr);
        exit(0);
      }
      if (arr[j] > arr[j + 1]) swap(&arr[j], &arr[j + 1]);
    }
  }
}

// Driver program to test above functions
int main() {
  srand(time(NULL));

  long long int max_seconds = 0;
  long long int array_size = 0;
  scanf("%lld", &max_seconds);
  scanf("%lld", &array_size);
  int *arr = (int *)malloc(sizeof(int) * array_size);
  for (long long int i = 0; i < array_size; ++i) {
    arr[i] = rand();
  }

  bubble_sort(arr, array_size, max_seconds);
  printf("Sorted array: \n");
  print_array(arr, array_size);
  free(arr);
  return 0;
}