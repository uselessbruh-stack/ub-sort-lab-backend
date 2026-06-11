#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#ifdef _WIN32
#include <windows.h>
#endif

#define MAX_ELEMENTS 50
#define MAX_VALUE 1000
#define MIN_RUN 32

/* ========================= HIGH PRECISION TIMING ========================= */
#ifdef _WIN32
double get_time_ms() {
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / frequency.QuadPart * 1000.0;
}
#else
double get_time_ms() {
    struct timespec ts;  
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}
#endif

/* ========================= TYPES & STRUCTURES ========================= */
typedef struct {
    int algorithm_id;
    char algorithm_name[32];
    int n;
    int *original;
    int *sorted;
    double actual_time;
    long comparisons;
    long swaps;
    double theoretical_ops;
} SortingResult;

/* ========================= UTILITY FUNCTIONS ========================= */
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void copyArray(int *src, int *dest, int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

void printArray(int *a, int n) {
    printf("[");
    for (int i = 0; i < n; i++) {
        printf("%d", a[i]);
        if (i < n - 1) printf(",");
    }
    printf("]");
}

int isSorted(int *a, int n) {
    for (int i = 0; i < n - 1; i++) {
        if (a[i] > a[i + 1]) return 0;
    }
    return 1;
}

/* ========================= SORTING ALGORITHMS ========================= */

/* 1. BUBBLE SORT - O(n²) */
void bubbleSort(int *a, int n, long *comparisons, long *swaps) {
    *comparisons = 0;
    *swaps = 0;
    
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - i - 1; j++) {
            (*comparisons)++;
            if (a[j] > a[j + 1]) {
                swap(&a[j], &a[j + 1]);
                (*swaps)++;
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

/* 2. SELECTION SORT - O(n²) */
void selectionSort(int *a, int n, long *comparisons, long *swaps) {
    *comparisons = 0;
    *swaps = 0;
    
    for (int i = 0; i < n - 1; i++) {
        int min = i;
        for (int j = i + 1; j < n; j++) {
            (*comparisons)++;
            if (a[j] < a[min]) {
                min = j;
            }
        }
        swap(&a[i], &a[min]);
        (*swaps)++;
    }
}

/* 3. INSERTION SORT - O(n²) */
void insertionSort(int *a, int n, long *comparisons, long *swaps) {
    *comparisons = 0;
    *swaps = 0;
    
    for (int i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0) {
            (*comparisons)++;
            if (a[j] > key) {
                a[j + 1] = a[j];
                (*swaps)++;
                j--;
            } else {
                break;
            }
        }
        if (j < i - 1) {
            a[j + 1] = key;
        }
    }
}

/* 4. MERGE SORT - O(n log n) */
void merge(int *a, int left, int mid, int right, long *comparisons, long *swaps) {
    int leftLen = mid - left + 1;
    int rightLen = right - mid;
    
    int *leftArr = (int *)malloc(leftLen * sizeof(int));
    int *rightArr = (int *)malloc(rightLen * sizeof(int));
    
    for (int i = 0; i < leftLen; i++)
        leftArr[i] = a[left + i];
    for (int j = 0; j < rightLen; j++)
        rightArr[j] = a[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    while (i < leftLen && j < rightLen) {
        (*comparisons)++;
        if (leftArr[i] <= rightArr[j]) {
            a[k++] = leftArr[i++];
        } else {
            a[k++] = rightArr[j++];
        }
        (*swaps)++;
    }
    
    while (i < leftLen) {
        a[k++] = leftArr[i++];
        (*swaps)++;
    }
    while (j < rightLen) {
        a[k++] = rightArr[j++];
        (*swaps)++;
    }
    
    free(leftArr);
    free(rightArr);
}

void mergeSortHelper(int *a, int left, int right, long *comparisons, long *swaps) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortHelper(a, left, mid, comparisons, swaps);
        mergeSortHelper(a, mid + 1, right, comparisons, swaps);
        merge(a, left, mid, right, comparisons, swaps);
    }
}

void mergeSort(int *a, int n, long *comparisons, long *swaps) {
    *comparisons = 0;
    *swaps = 0;
    mergeSortHelper(a, 0, n - 1, comparisons, swaps);
}

/* 5. QUICK SORT - O(n log n) average */
int partition(int *a, int low, int high, long *comparisons, long *swaps) {
    int pivot = a[high];
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        (*comparisons)++;
        if (a[j] < pivot) {
            i++;
            swap(&a[i], &a[j]);
            (*swaps)++;
        }
    }
    swap(&a[i + 1], &a[high]);
    (*swaps)++;
    return i + 1;
}

void quickSortHelper(int *a, int low, int high, long *comparisons, long *swaps) {
    if (low < high) {
        int pi = partition(a, low, high, comparisons, swaps);
        quickSortHelper(a, low, pi - 1, comparisons, swaps);
        quickSortHelper(a, pi + 1, high, comparisons, swaps);
    }
}

void quickSort(int *a, int n, long *comparisons, long *swaps) {
    *comparisons = 0;
    *swaps = 0;
    quickSortHelper(a, 0, n - 1, comparisons, swaps);
}

/* 6. HEAP SORT - O(n log n) */
void heapify(int *a, int n, int i, long *comparisons, long *swaps) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < n) {
        (*comparisons)++;
        if (a[left] > a[largest])
            largest = left;
    }
    if (right < n) {
        (*comparisons)++;
        if (a[right] > a[largest])
            largest = right;
    }
    
    if (largest != i) {
        swap(&a[i], &a[largest]);
        (*swaps)++;
        heapify(a, n, largest, comparisons, swaps);
    }
}

void heapSort(int *a, int n, long *comparisons, long *swaps) {
    *comparisons = 0;
    *swaps = 0;
    
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(a, n, i, comparisons, swaps);
    
    for (int i = n - 1; i > 0; i--) {
        swap(&a[0], &a[i]);
        (*swaps)++;
        heapify(a, i, 0, comparisons, swaps);
    }
}

/* 7. COUNTING SORT - O(n + k) */
void countingSort(int *a, int n, long *comparisons, long *swaps) {
    *comparisons = 0;
    *swaps = 0;
    
    if (n <= 1) return;
    
    int max = a[0];
    for (int i = 1; i < n; i++) {
        if (a[i] > max) max = a[i];
    }
    
    int *count = (int *)calloc(max + 1, sizeof(int));
    
    for (int i = 0; i < n; i++) {
        count[a[i]]++;
        (*comparisons)++;
    }
    
    int idx = 0;
    for (int i = 0; i <= max; i++) {
        for (int j = 0; j < count[i]; j++) {
            a[idx++] = i;
            (*swaps)++;
        }
    }
    
    free(count);
}

/* 8. RADIX SORT - O(nk) where k is number of digits */
void countingSortForRadix(int *a, int n, int exp, long *comparisons, long *swaps) {
    int *output = (int *)malloc(n * sizeof(int));
    int count[10] = {0};
    
    for (int i = 0; i < n; i++) {
        count[(a[i] / exp) % 10]++;
        (*comparisons)++;
    }
    
    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }
    
    for (int i = n - 1; i >= 0; i--) {
        output[count[(a[i] / exp) % 10] - 1] = a[i];
        count[(a[i] / exp) % 10]--;
        (*swaps)++;
    }
    
    for (int i = 0; i < n; i++) {
        a[i] = output[i];
    }
    
    free(output);
}

void radixSort(int *a, int n, long *comparisons, long *swaps) {
    *comparisons = 0;
    *swaps = 0;
    
    int max = a[0];
    for (int i = 1; i < n; i++) {
        if (a[i] > max) max = a[i];
    }
    
    for (int exp = 1; max / exp > 0; exp *= 10) {
        countingSortForRadix(a, n, exp, comparisons, swaps);
    }
}

/* 9. BUCKET SORT - O(n + k) */
void bucketSort(int *a, int n, long *comparisons, long *swaps) {
    *comparisons = 0;
    *swaps = 0;
    
    if (n <= 1) return;
    
    int max = a[0];
    for (int i = 1; i < n; i++) {
        if (a[i] > max) max = a[i];
    }
    
    int buckets = (max / n) + 1;
    int **bucketArray = (int **)malloc(buckets * sizeof(int *));
    for (int i = 0; i < buckets; i++) {
        bucketArray[i] = (int *)malloc(n * sizeof(int));
    }
    int *bucketCount = (int *)calloc(buckets, sizeof(int));
    
    for (int i = 0; i < n; i++) {
        int idx = a[i] / (max / buckets + 1);
        if (idx >= buckets) idx = buckets - 1;
        bucketArray[idx][bucketCount[idx]++] = a[i];
    }
    
    for (int i = 0; i < buckets; i++) {
        for (int j = 0; j < bucketCount[i] - 1; j++) {
            for (int k = 0; k < bucketCount[i] - j - 1; k++) {
                (*comparisons)++;
                if (bucketArray[i][k] > bucketArray[i][k + 1]) {
                    swap(&bucketArray[i][k], &bucketArray[i][k + 1]);
                    (*swaps)++;
                }
            }
        }
    }
    
    int idx = 0;
    for (int i = 0; i < buckets; i++) {
        for (int j = 0; j < bucketCount[i]; j++) {
            a[idx++] = bucketArray[i][j];
        }
        free(bucketArray[i]);
    }
    free(bucketArray);
    free(bucketCount);
}

/* 10. TIM SORT - O(n log n) */
void timInsertionSort(int *a, int left, int right, long *comparisons, long *swaps) {
    for (int i = left + 1; i <= right; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= left) {
            (*comparisons)++;
            if (a[j] > key) {
                a[j + 1] = a[j];
                (*swaps)++;
                j--;
            } else {
                break;
            }
        }
        a[j + 1] = key;
    }
}

void timMerge(int *a, int left, int mid, int right, long *comparisons, long *swaps) {
    int leftLen = mid - left + 1;
    int rightLen = right - mid;
    
    int *leftArr = (int *)malloc(leftLen * sizeof(int));
    int *rightArr = (int *)malloc(rightLen * sizeof(int));
    
    for (int i = 0; i < leftLen; i++)
        leftArr[i] = a[left + i];
    for (int j = 0; j < rightLen; j++)
        rightArr[j] = a[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    while (i < leftLen && j < rightLen) {
        (*comparisons)++;
        if (leftArr[i] <= rightArr[j]) {
            a[k++] = leftArr[i++];
        } else {
            a[k++] = rightArr[j++];
        }
        (*swaps)++;
    }
    
    while (i < leftLen) {
        a[k++] = leftArr[i++];
    }
    while (j < rightLen) {
        a[k++] = rightArr[j++];
    }
    
    free(leftArr);
    free(rightArr);
}

void timSortHelper(int *a, int n, long *comparisons, long *swaps) {
    int minRun = MIN_RUN;
    
    for (int i = 0; i < n; i += minRun) {
        int right = (i + minRun - 1 < n - 1) ? i + minRun - 1 : n - 1;
        timInsertionSort(a, i, right, comparisons, swaps);
    }
    
    for (int size = minRun; size < n; size *= 2) {
        for (int i = 0; i < n; i += size * 2) {
            int mid = i + size - 1;
            int right = (i + size * 2 - 1 < n - 1) ? i + size * 2 - 1 : n - 1;
            if (mid < right) {
                timMerge(a, i, mid, right, comparisons, swaps);
            }
        }
    }
}

void timSort(int *a, int n, long *comparisons, long *swaps) {
    *comparisons = 0;
    *swaps = 0;
    timSortHelper(a, n, comparisons, swaps);
}

/* 11. INTRO SORT - O(n log n) */
int introPart(int *a, int low, int high, long *comparisons, long *swaps) {
    int pivot = a[high];
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        (*comparisons)++;
        if (a[j] < pivot) {
            i++;
            swap(&a[i], &a[j]);
            (*swaps)++;
        }
    }
    swap(&a[i + 1], &a[high]);
    (*swaps)++;
    return i + 1;
}

void introSortHelper(int *a, int low, int high, int depthLimit, long *comparisons, long *swaps) {
    if (low < high) {
        if (depthLimit == 0) {
            heapSort(a + low, high - low + 1, comparisons, swaps);
        } else {
            int pi = introPart(a, low, high, comparisons, swaps);
            introSortHelper(a, low, pi - 1, depthLimit - 1, comparisons, swaps);
            introSortHelper(a, pi + 1, high, depthLimit - 1, comparisons, swaps);
        }
    }
}

void introSort(int *a, int n, long *comparisons, long *swaps) {
    *comparisons = 0;
    *swaps = 0;
    int depthLimit = 2 * log(n);
    introSortHelper(a, 0, n - 1, depthLimit, comparisons, swaps);
}



/* ========================= MAIN FUNCTION ========================= */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <algorithm> [values...]\n", argv[0]);
        fprintf(stderr, "Algorithms: bubble, selection, insertion, merge, quick, heap, counting, radix, bucket, tim, intro\n");
        fprintf(stderr, "Constraints: Max %d elements, each 0-%d\n", MAX_ELEMENTS, MAX_VALUE);
        fprintf(stderr, "If values provided: sorts those values and outputs result\n");
        fprintf(stderr, "If no values: tests algorithm with sizes 5-50 and outputs performance data\n");
        return 1;
    }
    
    char *algorithm = argv[1];
    
    /* Validate algorithm */
    if (strcmp(algorithm, "bubble") != 0 && strcmp(algorithm, "selection") != 0 && 
        strcmp(algorithm, "insertion") != 0 && strcmp(algorithm, "merge") != 0 &&
        strcmp(algorithm, "quick") != 0 && strcmp(algorithm, "heap") != 0 &&
        strcmp(algorithm, "counting") != 0 && strcmp(algorithm, "radix") != 0 &&
        strcmp(algorithm, "bucket") != 0 && strcmp(algorithm, "tim") != 0 &&
        strcmp(algorithm, "intro") != 0) {
        fprintf(stderr, "Unknown algorithm: %s\n", algorithm);
        return 1;
    }
    
    /* MODE 1: User provided values */
    if (argc > 2) {
        int n = argc - 2;
        
        if (n > MAX_ELEMENTS) {
            fprintf(stderr, "Error: Maximum %d elements allowed\n", MAX_ELEMENTS);
            return 1;
        }
        
        int *arr = (int *)malloc(n * sizeof(int));
        int *sorted_arr = (int *)malloc(n * sizeof(int));
        
        for (int i = 0; i < n; i++) {
            arr[i] = atoi(argv[2 + i]);
            if (arr[i] < 0 || arr[i] > MAX_VALUE) {
                fprintf(stderr, "Error: Element values must be between 0 and %d\n", MAX_VALUE);
                free(arr);
                free(sorted_arr);
                return 1;
            }
        }
        
        /* Sort the array and collect statistics */
        copyArray(arr, sorted_arr, n);
        double start = get_time_ms();
        long comparisons = 0, swaps = 0;
        
        if (strcmp(algorithm, "bubble") == 0) {
            bubbleSort(sorted_arr, n, &comparisons, &swaps);
        } else if (strcmp(algorithm, "selection") == 0) {
            selectionSort(sorted_arr, n, &comparisons, &swaps);
        } else if (strcmp(algorithm, "insertion") == 0) {
            insertionSort(sorted_arr, n, &comparisons, &swaps);
        } else if (strcmp(algorithm, "merge") == 0) {
            mergeSort(sorted_arr, n, &comparisons, &swaps);
        } else if (strcmp(algorithm, "quick") == 0) {
            quickSort(sorted_arr, n, &comparisons, &swaps);
        } else if (strcmp(algorithm, "heap") == 0) {
            heapSort(sorted_arr, n, &comparisons, &swaps);
        } else if (strcmp(algorithm, "counting") == 0) {
            countingSort(sorted_arr, n, &comparisons, &swaps);
        } else if (strcmp(algorithm, "radix") == 0) {
            radixSort(sorted_arr, n, &comparisons, &swaps);
        } else if (strcmp(algorithm, "bucket") == 0) {
            bucketSort(sorted_arr, n, &comparisons, &swaps);
        } else if (strcmp(algorithm, "tim") == 0) {
            timSort(sorted_arr, n, &comparisons, &swaps);
        } else if (strcmp(algorithm, "intro") == 0) {
            introSort(sorted_arr, n, &comparisons, &swaps);
        }
        
        double end = get_time_ms();
        double time_taken = end - start;
        
        /* Output JSON result */
        printf("{\"algorithm\":\"%s\",\"n\":%d,\"input\":", algorithm, n);
        printArray(arr, n);
        printf(",\"output\":");
        printArray(sorted_arr, n);
        printf(",\"actual_time_ms\":%.6f,\"comparisons\":%ld,\"swaps\":%ld,\"is_sorted\":%s}\n",
               time_taken, comparisons, swaps, isSorted(sorted_arr, n) ? "true" : "false");
        
        free(arr);
        free(sorted_arr);
        
        return 0;
    }
    
    /* MODE 2: Performance testing with varying sizes */
    srand(time(NULL));
    
    for (int size = 5; size <= MAX_ELEMENTS; size += 5) {
        int *arr = (int *)malloc(size * sizeof(int));
        int *sorted_arr = (int *)malloc(size * sizeof(int));
        
        /* Generate random array */
        for (int i = 0; i < size; i++) {
            arr[i] = rand() % (MAX_VALUE + 1);
        }
        
        copyArray(arr, sorted_arr, size);
        
        double start = get_time_ms();
        long comparisons = 0, swaps = 0;
        
        if (strcmp(algorithm, "bubble") == 0) {
            bubbleSort(sorted_arr, size, &comparisons, &swaps);
        } else if (strcmp(algorithm, "selection") == 0) {
            selectionSort(sorted_arr, size, &comparisons, &swaps);
        } else if (strcmp(algorithm, "insertion") == 0) {
            insertionSort(sorted_arr, size, &comparisons, &swaps);
        } else if (strcmp(algorithm, "merge") == 0) {
            mergeSort(sorted_arr, size, &comparisons, &swaps);
        } else if (strcmp(algorithm, "quick") == 0) {
            quickSort(sorted_arr, size, &comparisons, &swaps);
        } else if (strcmp(algorithm, "heap") == 0) {
            heapSort(sorted_arr, size, &comparisons, &swaps);
        } else if (strcmp(algorithm, "counting") == 0) {
            countingSort(sorted_arr, size, &comparisons, &swaps);
        } else if (strcmp(algorithm, "radix") == 0) {
            radixSort(sorted_arr, size, &comparisons, &swaps);
        } else if (strcmp(algorithm, "bucket") == 0) {
            bucketSort(sorted_arr, size, &comparisons, &swaps);
        } else if (strcmp(algorithm, "tim") == 0) {
            timSort(sorted_arr, size, &comparisons, &swaps);
        } else if (strcmp(algorithm, "intro") == 0) {
            introSort(sorted_arr, size, &comparisons, &swaps);
        }
        
        double end = get_time_ms();
        double time_taken = end - start;
        printf("{\"algorithm\":\"%s\",\"n\":%d,\"actual_time_ms\":%.6f,\"comparisons\":%ld,\"swaps\":%ld}\n",
               algorithm, size, time_taken, comparisons, swaps);
        
        free(arr);
        free(sorted_arr);
    }
    
    return 0;
}
