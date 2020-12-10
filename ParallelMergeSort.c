#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SEED 100
#define LENGTH 100
#define UPPER_LIM 10000
#define LOWER_LIM 1
#define NUM_THREADS 2

void printArray(int arr[], int SIZE){ 
     printf("\n");
     for(int i = 0; i<SIZE; i++) printf("%5d", arr[i]);
     printf("\n");
}

void fillArrayWithRandomNumbers(int arr[], int SIZE){
     for(int i = 0; i<SIZE; i++)
       arr[i] = rand()%100;
}

const int NUMBERS_PER_THREAD = LENGTH / NUM_THREADS;
const int OFFSET = LENGTH % NUM_THREADS;
int arr[LENGTH];

void merge(int arr[], int left, int middle, int right){
    int i = 0;
    int j = 0;
    int k = 0;
    int left_length = middle - left + 1;
    int right_length = right - middle;
    int left_array[left_length];
    int right_array[right_length];
    
    for(int j = 0; j < right_length; j ++)
        right_array[j] = arr[middle + 1 + j];
  
    for(int i = 0; i < left_length; i ++)
        left_array[i] = arr[left + i];
    
    i = 0;
    j = 0;
    while(i < left_length && j < right_length){
        if (left_array[i] <= right_array[j]){
            arr[left + k] = left_array[i];
            i ++;
        }else{
            arr[left + k] = right_array[j];
            j ++;
        }
        k ++;
    }
    
    while(i < left_length){
        arr[left + k] = left_array[i];
        k ++;
        i ++;
    }
    while(j < right_length){
        arr[left + k] = right_array[j];
        k ++;
        j ++;
    }
}

void merge_sections_of_array(int arr[], int number, int aggregation){
    for(int i = 0; i < number; i = i + 2) {
        int left = i * (NUMBERS_PER_THREAD * aggregation);
        int right = ((i + 2) * NUMBERS_PER_THREAD * aggregation) - 1;
        int middle = left + (NUMBERS_PER_THREAD * aggregation) - 1;
        if (right >= LENGTH){
            right = LENGTH - 1;
        }
        merge(arr, left, middle, right);
    }
    if(number / 2 >= 1){
        merge_sections_of_array(arr, number / 2, aggregation * 2);
    }
}

void merge_sort(int arr[], int left, int right){
    if(left < right){
        int middle = left + (right - left) / 2;
        merge_sort(arr, left, middle);
        merge_sort(arr, middle + 1, right);
        merge(arr, left, middle, right);
    }
}

void *thread_merge_sort(void* arg){
    int thread_id = (long)arg;
    int left = thread_id * (NUMBERS_PER_THREAD);
    int right = (thread_id + 1) * (NUMBERS_PER_THREAD) - 1;
    if(thread_id == NUM_THREADS - 1){
        right += OFFSET;
    }
    int middle = left + (right - left) / 2;
    if(left < right){
        merge_sort(arr, left, right);
        merge_sort(arr, left + 1, right);
        merge(arr, left, middle, right);
    }
    return NULL;
}

void orderCheck(int arr[]){
    int max = 0;
    for(int i = 1; i < LENGTH; i ++){
        if (arr[i] >= arr[i - 1]){
            max = arr[i];
        }else{
            printf("The array is out of order at arr[%d]: %d\n", i, arr[i]);
            return;
        }
    }
    printf("The array is sorted\n");
}

int main(int argc, const char * argv[]){
    srand(SEED);
    
    fillArrayWithRandomNumbers(arr, sizeof arr);
    printf("Unsorted array:   ");
    printArray(arr, sizeof arr);
    pthread_t threads[NUM_THREADS];
    
    for (long i = 0; i < NUM_THREADS; i++){
        int rc = pthread_create(&threads[i], NULL, thread_merge_sort, (void *)i);
        if (rc){
            printf("ERROR");
            exit(-1);
        }
    }
    
    for(long i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }
    
    merge_sections_of_array(arr, NUM_THREADS, 1);
    //arr[50] = -10;
    printf("Sorted array:   ");
    printArray(arr, sizeof arr);
    
    orderCheck(arr);
    return 0;
}