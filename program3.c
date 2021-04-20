#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define N_BUFFER_SIZE 1024

void merge(int i, int j);
void* mergesort(void *a);

int array[INT_MAX];              // Global(Shared) array for storing sorted numbers from each threads

typedef struct context_tracker {
	// Struct that contains relevant info like indices or current number of threads
	// Every struct has the same total_thread_num and cur_thread_num
	// The only thing that varies between instances of context_tracker is i, j (indices) of where to sort
    int left;
    int right;
    int total_thread_num;
	int cur_thread_num;
} TRACKER;


int main(int argc, char *argv[]) {

	// 1. Check if proper argument has been passed
    if (argc != 2) {
        perror("Should only pass [total_thread_num]\n");
        exit(1);
    }

    // 2. Initialize variables we need 
    // Use shared data (array, context_tracker) to update the given array and number of threads that is spawned
    clock_t start_time, end_time;
    char N_buffer[N_BUFFER_SIZE];
    TRACKER tracker;
    pthread_t tid;
    
	// 3. Process Input
	fgets(N_buffer, N_BUFFER_SIZE, stdin);
	int length = atoi(N_buffer);

	for (int i = 0; i < length; i++) {
		fscanf(stdin, "%d", &(array[i]));
		fflush(stdin);
	}

	tracker.total_thread_num = atoi(argv[1]);
    tracker.cur_thread_num = 0;
	tracker.left = 0;
    tracker.right = length - 1;

	// 4. Mergesort using MultiThreading
    start_time = clock();
    if (pthread_create(&tid, NULL, mergesort, &tracker) < 0) {
		perror("pthread_create() Failed\n");
        exit(1);
    }
    pthread_join(tid, NULL);
    end_time = clock();

    // 5. Redirect the output to standard output
    for (int i = 0; i < length; i++) {
        fprintf(stdout, "%d ", array[i]);
    }

    fprintf(stdout, "\n");
    fprintf(stdout, "%d\n", (int)(end_time - start_time));

    return 0;
}


void* mergesort(void *a) {

    TRACKER *tracker_ptr = (TRACKER *)a;
    if (tracker_ptr->left >= tracker_ptr->right) {
	  	return 0;
	}
	int mid = (tracker_ptr->left + tracker_ptr->right) / 2;

    // Since we spwan 2 threads at an iteration, check if there are enough threads left
    // Case #1 : If we cannot spawn more threads, just mergesort in a serial manner
    if (tracker_ptr->total_thread_num <= tracker_ptr->cur_thread_num || tracker_ptr->total_thread_num == tracker_ptr->cur_thread_num + 1) {

		if (tracker_ptr->total_thread_num == tracker_ptr->cur_thread_num + 1) {
			tracker_ptr->cur_thread_num += 1;
		}

	    // Initialize tracker1 for left part
	    TRACKER tracker1;
	    tracker1.left = tracker_ptr->left;
	    tracker1.right = mid;
	    tracker1.total_thread_num = tracker_ptr->total_thread_num;
	    tracker1.cur_thread_num = tracker_ptr->cur_thread_num;

	    // Initialize tracker2 for right part
	    TRACKER tracker2;
	    tracker2.left = mid + 1;
	    tracker2.right = tracker_ptr->right;
	    tracker2.total_thread_num = tracker_ptr->total_thread_num;
	    tracker2.cur_thread_num = tracker_ptr->cur_thread_num;

	    mergesort(&tracker1);
	    mergesort(&tracker2);
	    merge(tracker_ptr->left, tracker_ptr->right);


    } else {

		// Case #2 : If we can spawn more threads, spawn 2 threads
	    pthread_t left_tid, right_tid;
	    
	    tracker_ptr->cur_thread_num += 2;
	    
	    // Initialize tracker1 for left part
	    TRACKER tracker1;
	    tracker1.left = tracker_ptr->left;
	    tracker1.right = mid;
	    tracker1.total_thread_num = tracker_ptr->total_thread_num;
	    tracker1.cur_thread_num = tracker_ptr->cur_thread_num;

	    // Initialize tracker2 for right part
	    TRACKER tracker2;
	    tracker2.left = mid + 1;
	    tracker2.right = tracker_ptr->right;
	    tracker2.total_thread_num = tracker_ptr->total_thread_num;
	    tracker2.cur_thread_num = tracker_ptr->cur_thread_num;

	    // Create a thread for the left part
	    if (pthread_create(&left_tid, NULL, mergesort, &tracker1) < 0) {            
	    	perror("pthread_create() failed\n");
	        exit(1);
	    }

	    // Create a thread for the right part
	    if (pthread_create(&right_tid, NULL, mergesort, &tracker2) < 0) {
	        perror("pthread_create() failed\n");
	        exit(1);
	    }

	    // Join the threads (wait)
	    pthread_join(left_tid, NULL);
	    pthread_join(right_tid, NULL);

	    merge(tracker_ptr->left, tracker_ptr->right);
	    pthread_exit(NULL);

	}
}

void merge(int left, int right2) {

	int right1 = (left + right2) / 2;
    int length = right2 - left + 1;
    int tmp_sorted[length];
    int idx1 = left;
    int idx2 = right1 + 1;
    int dest_idx = 0;

    // 1. Iterate each part and globally sort by merging 2 subarrays
    while (idx1 <= right1 && idx2 <= right2) {

        if (array[idx1] < array[idx2]) {
            tmp_sorted[dest_idx++] = array[idx2++];

        } else if (array[idx1] > array[idx2]) {
            tmp_sorted[dest_idx++] = array[idx1++];

        } else if (array[idx1] == array[idx2]) {
            tmp_sorted[dest_idx++] = array[idx1++];
            tmp_sorted[dest_idx++] = array[idx2++];
        }
    }

    // 2. Process leftovers
    // Case #1 - When first array is left
    while (idx1 <= right1) {
        tmp_sorted[dest_idx++] = array[idx1++];
    }

    // Case #2 - When second array is left
    while (idx2 <= right2) {
        tmp_sorted[dest_idx++] = array[idx2++];
    }

    // 3. Update the sorted array to global array
    for (int i = 0; i < length; i++) {
        array[left] = tmp_sorted[i];
        left++;
    }

}