#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N_BUFFER_SIZE 1024 // Buffer size for the first input (N)

void merge(int arr[], int left1, int right1, int right2);
void mergeSort(int arr[], int low, int high);

int main(int argc, char *argv[]) {

    // 1. Initialize variables we need
    clock_t start, end;
    char N_buffer[N_BUFFER_SIZE];
    
    // 2. Process Input
    fgets(N_buffer, N_BUFFER_SIZE, stdin);
    int N = atoi(N_buffer);
    int numbers[N];
    
    for (int i = 0; i < N; i++) {
        fscanf(stdin, "%d", &(numbers[i]));
        fflush(stdin);
    }

    // 3. Mergesort (Serial)
    start = clock();
    mergeSort(numbers, 0, N - 1);
    end = clock();

    // 4. Redirect the output to standard output
    for (int i = 0; i < N; i++) {
        fprintf(stdout, "%d ", numbers[i]);
    }

    fprintf(stdout, "\n");
    fprintf(stdout, "%d\n", (int)(end - start));
    return 0;

}


void mergeSort(int arr[], int left, int right) {
    if (left < right) {
	    int length = (right - left + 1);
	 	mergeSort(arr, left, left + length / 2 - 1);
	    mergeSort(arr, left + length / 2, right);
	    merge(arr, left, left + length / 2 - 1, right);
	}
}
 

void merge(int arr[], int left, int right1, int right2) {

    int length = right2 - left + 1;
    int tmp_sorted[length];
    int idx1 = left;
    int idx2 = right1 + 1;
    int dest_idx = 0;

    // 1. Iterate each part and globally sort by merging 2 subarrays
    while (idx1 <= right1 && idx2 <= right2) {

        if (arr[idx1] > arr[idx2]) {
            tmp_sorted[dest_idx++] = arr[idx1++];

        } else if (arr[idx2] > arr[idx1]) {
            tmp_sorted[dest_idx++] = arr[idx2++];

        } else if (arr[idx1] == arr[idx2]) {
            tmp_sorted[dest_idx++] = arr[idx1++];
            tmp_sorted[dest_idx++] = arr[idx2++];
        }
    }

    // 2. Process leftovers
    // Case #1 - When first array is left
    while (idx1 <= right1) {
        tmp_sorted[dest_idx++] = arr[idx1++];
    }
    
    // Case #2 - When second array is left
    while (idx2 <= right2) {
        tmp_sorted[dest_idx++] = arr[idx2++];
    }
    
    // 3. Update the sorted array to global array
    for (int i = 0; i < length; i++) {
        arr[left] = tmp_sorted[i];
        left++;
    }
}

