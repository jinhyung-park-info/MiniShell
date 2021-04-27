#include <sys/types.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

#define N_BUFFER_SIZE 1024  // Buffer size for the first input (N)

void processInput(int arr[], int len);
void merge(int arr[], int low1, int high1, int high2);
void mergeSort(int arr[], int low, int high, int tracker[]);

int main(int argc, char *argv[]) {

    // 1. Check if proper argument has been passed
    if (argc != 2) {
        perror("Should only pass [total_process_num]\n");
        _exit(1);
    }

    // 2. Initialize variables we need 
    // Use shared data to update the given array and number of processes that is spawned
    clock_t start_time, end_time;
    char N_buffer[N_BUFFER_SIZE];
    fgets(N_buffer, N_BUFFER_SIZE, stdin);
    int length = atoi(N_buffer);

    int numbers_array_id, process_tracker_id;
    key_t array_key = IPC_PRIVATE;
    key_t process_tracker_key = IPC_PRIVATE;
    int *numbers_array, *process_tracker;

    size_t ARRAY_SHM_SIZE = sizeof(int) * length;
    size_t TRACKER_SHM_SIZE = sizeof(int) * 2;
 
    // 3. Allocate segment and attach for our shared data
    if ((numbers_array_id = shmget(array_key, ARRAY_SHM_SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        _exit(1);
    }

    if ((process_tracker_id = shmget(process_tracker_key, TRACKER_SHM_SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        _exit(1);
    }
 
    if ((numbers_array = shmat(numbers_array_id, NULL, 0)) == (int *) -1) {
        perror("shmat");
        _exit(1);
    }

    if ((process_tracker = shmat(process_tracker_id, NULL, 0)) == (int*) -1) {
        perror("shmat");
        _exit(1);
    }

    // 4. Process Input
    process_tracker[0] = atoi(argv[1]);  // total num processes
    process_tracker[1] = 0;              // current num processes
    processInput(numbers_array, length); // input array

    // 5. MergeSort using MultiProcessing
    start_time = clock();
    mergeSort(numbers_array, 0, length - 1, process_tracker);
    end_time = clock();

    // 6. Redirect the output to standard output
    for (int i = 0; i < length; i++) {
        fprintf(stdout, "%d ", numbers_array[i]);
    }

    fprintf(stdout, "\n");
    fprintf(stdout, "%d\n", (int)(end_time - start_time));

    // 7. Free and detach the shared memory
    if (shmdt(numbers_array) == -1) {
        perror("shmdt");
        _exit(1);
    }

    if (shmdt(process_tracker) == -1) {
        perror("shmdt");
        _exit(1);
    }
 
    if (shmctl(numbers_array_id, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        _exit(1);
    }

    if (shmctl(process_tracker_id, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        _exit(1);
    }

    return 0;
}


void mergeSort(int arr[], int left, int right, int tracker[]) {

    if (left < right) {

        int length = right - left + 1;

        // Since we fork 2 processes at an iteration, check if there are enough processes left
        // Case #1 : If we can spawn more processes
        if (tracker[0] > tracker[1] + 1) {
     
            pid_t left_pid, right_pid;
            left_pid = fork();
            tracker[1] += 2;

            if (left_pid < 0) {
                perror("fork() failed\n");
                _exit(-1);

            } else if (left_pid == 0) {
                mergeSort(arr, left, left + length / 2 - 1, tracker);
                _exit(0);

            } else {

                right_pid = fork();

                if (right_pid < 0) {
                    perror("fork() Failed\n");
                    _exit(-1);

                } else if (right_pid == 0) {
                    mergeSort(arr, left + length / 2, right, tracker);
                    _exit(0);
                }
            }
         
            int status;
         
            // Wait for child processes to finish
            waitpid(left_pid, &status, 0);
            waitpid(right_pid, &status, 0);

        } else {

            // Case #2 : If we cannot spawn more processes, just mergesort in a serial manner
            if (tracker[0] == tracker[1] + 1) {
                tracker[1] += 1;
            }

            mergeSort(arr, left, left + length / 2 - 1, tracker);
            mergeSort(arr, left + length / 2, right, tracker);
        }

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
 

void processInput(int arr[], int len) {
    for (int i = 0; i < len; i++) {
        fscanf(stdin, "%d", &(arr[i]));
        fflush(stdin);
    }
    return;
}
