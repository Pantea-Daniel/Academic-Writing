#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/**
 * Program to benchmark various sorting algorithms on different data sets.
 * Implements bubble sort, quick sort, insertion sort, selection sort, merge sort, and radix sort.
 * Measures performance on random, reversed, and partially sorted arrays.
 */

#define BUBBLE_SORT 0
#define QUICK_SORT 1
#define INSERTION_SORT 2
#define SELECTION_SORT 3
#define MERGE_SORT 4
#define RADIX_SORT 5

static const char* ALGORITHM_NAMES[] = {
    "bubble sort",
    "quick sort",
    "insertion sort",
    "selection sort",
    "merge sort",
    "radix sort"
};

static FILE* resultsFile = NULL;

void resetRandomSeed() { srand((unsigned int)time(NULL)); }

clock_t startOperationTimer() { return clock(); }

double stopOperationTimerMs(clock_t startTime) {
    clock_t endTime = clock();
    return ((double)(endTime - startTime) * 1000.0) / CLOCKS_PER_SEC;
}

// Generates a random integer array of given size
int* generateIntArray(unsigned long size) {
    int* array = malloc(sizeof(int) * size);
    for (unsigned long i = 0; i < size; i++) {
        array[i] = (int)(rand() % (size + 1));
    }
    return array;
}

// Generates a partially sorted array by starting with sorted and swapping a few elements
int* generatePartiallySortedIntArray(unsigned long size, int factor) {
    int* array = malloc(sizeof(int) * size);
    for (unsigned long i = 0; i < size; i++) {
        array[i] = (int)i;
    }
    unsigned long swapCount = size / factor;
    if (swapCount == 0 && size > 1) swapCount = 1;
    for (unsigned long i = 0; i < swapCount; i++) {
        unsigned long indexA = (unsigned long)(rand() % size);
        unsigned long indexB = (unsigned long)(rand() % size);
        int temp = array[indexA];
        array[indexA] = array[indexB];
        array[indexB] = temp;
    }
    return array;
}

// Generates a reverse sorted array
int* generateReverseSortedIntArray(unsigned long size) {
    int* array = malloc(sizeof(int) * size);
    for (unsigned long i = 0; i < size; i++) {
        array[i] = (int)(size - 1 - i);
    }
    return array;
}

// Swaps two integers
void swapInts(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}


/* Selects the median of the first, middle, and last elements to use as pivot.
   This prevents O(n^2) on sorted/reversed data. */
void selectMedianPivot(int* array, long low, long high) {
    long mid = low + (high - low) / 2;
    if (array[low] > array[mid]) swapInts(&array[low], &array[mid]);
    if (array[low] > array[high]) swapInts(&array[low], &array[high]);
    if (array[mid] > array[high]) swapInts(&array[mid], &array[high]);          // you take the low elem, the mid one and the high one and chose the one in the middle
    // Move median to high for partitioning                                           // ex {10, 50, 20}, you take 20 as a pivot
    swapInts(&array[mid], &array[high]);                                        // array[0] = low; array[lenght/2] = mid; array[-1] = high
}

long partitionQuickSort(int* array, long low, long high) {
    selectMedianPivot(array, low, high); // Pivot is now at array[high]
    int pivot = array[high];
    long i = low - 1;
    for (long j = low; j < high; j++) {
        if (array[j] <= pivot) {
            i++;
            swapInts(&array[i], &array[j]);
        }
    }
    swapInts(&array[i + 1], &array[high]);
    return i + 1;
}

void quickSortRecursive(int* array, long low, long high) {
    if (low < high) {
        long pivotIndex = partitionQuickSort(array, low, high);
        quickSortRecursive(array, low, pivotIndex - 1);
        quickSortRecursive(array, pivotIndex + 1, high);
    }
}

void quickSort(int* array, unsigned long size) {
    if (array == NULL || size < 2) return;
    quickSortRecursive(array, 0, (long)size - 1);
}

// Bubble sort: repeatedly steps through the list, compares adjacent elements and swaps them if they are in the wrong order
void bubbleSort(int* array, unsigned long size) {
    if (array == NULL || size < 2) return;
    for (unsigned long i = 0; i < size - 1; i++) {
        int swapped = 0;
        for (unsigned long j = 0; j < size - 1 - i; j++) {
            if (array[j] > array[j + 1]) {
                swapInts(&array[j], &array[j + 1]);
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

// Insertion sort: builds the final sorted array one item at a time
void insertionSort(int* array, unsigned long size) {
    if (array == NULL || size < 2) return;
    for (unsigned long i = 1; i < size; i++) {
        int key = array[i];
        long j = (long)i - 1;
        while (j >= 0 && array[j] > key) {
            array[j + 1] = array[j];
            j--;
        }
        array[j + 1] = key;
    }
}

// Selection sort: repeatedly finds the minimum element from the unsorted portion and puts it at the beginning
void selectionSort(int* array, unsigned long size) {
    if (array == NULL || size < 2) return;
    for (unsigned long i = 0; i < size - 1; i++) {
        unsigned long minIndex = i;
        for (unsigned long j = i + 1; j < size; j++) {
            if (array[j] < array[minIndex]) minIndex = j;
        }
        if (minIndex != i) swapInts(&array[i], &array[minIndex]);
    }
}

// Merges two sorted subarrays into one sorted array
void merge(int* array, unsigned long left, unsigned long mid, unsigned long right) {
    unsigned long n1 = mid - left + 1;
    unsigned long n2 = right - mid;
    int* L = malloc(sizeof(int) * n1);
    int* R = malloc(sizeof(int) * n2);
    for (unsigned long i = 0; i < n1; i++)
        L[i] = array[left + i];
    for (unsigned long j = 0; j < n2; j++)
        R[j] = array[mid + 1 + j];
    unsigned long i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) array[k++] = L[i++];
        else array[k++] = R[j++];
    }
    while (i < n1) array[k++] = L[i++];
    while (j < n2) array[k++] = R[j++];
    free(L); free(R);
}

// Recursive merge sort implementation
void mergeSortRecursive(int* array, long left, long right) {
    if (left < right) {
        long mid = left + (right - left) / 2;
        mergeSortRecursive(array, left, mid);
        mergeSortRecursive(array, mid + 1, right);
        merge(array, (unsigned long)left, (unsigned long)mid, (unsigned long)right);
    }
}

// Public interface for merge sort
void mergeSort(int* array, unsigned long size) {
    if (array == NULL || size < 2) return;
    mergeSortRecursive(array, 0, (long)size - 1);
}

// Finds the maximum value in the array for radix sort
int getMaxValue(int* array, unsigned long size) {
    int mx = array[0];
    for (unsigned long i = 1; i < size; i++)
        if (array[i] > mx)
            mx = array[i];
    return mx;
}

// Sorts the array by a specific digit place using counting sort
void countSortByDigit(int* array, unsigned long size, int exp) {
    int* output = malloc(sizeof(int) * size);
    int count[10] = {0};
    for (unsigned long i = 0; i < size; i++) count[(array[i] / exp) % 10]++;
    for (int i = 1; i < 10; i++) count[i] += count[i - 1];
    for (long i = (long)size - 1; i >= 0; i--) {
        output[count[(array[i] / exp) % 10] - 1] = array[i];
        count[(array[i] / exp) % 10]--;
    }
    for (unsigned long i = 0; i < size; i++)
        array[i] = output[i];
    free(output);
}

// Radix sort: sorts numbers by processing individual digits from least significant to most
void radixSort(int* array, unsigned long size) {
    if (array == NULL || size < 2) return;
    int m = getMaxValue(array, size);
    for (int exp = 1; m / exp > 0; exp *= 10) countSortByDigit(array, size, exp);
}

// Measures the time taken to sort an array with the specified algorithm
double measureSortAlg(int type, int* data, unsigned long size) {
    clock_t startTime = startOperationTimer();
    switch (type) {
        case BUBBLE_SORT: bubbleSort(data, size); break; 
        case QUICK_SORT: quickSort(data, size); break;
        case INSERTION_SORT: insertionSort(data, size); break;
        case SELECTION_SORT: selectionSort(data, size); break;
        case MERGE_SORT: mergeSort(data, size); break;
        case RADIX_SORT: radixSort(data, size); break;
    }
    return stopOperationTimerMs(startTime);
}

// Prints the CSV header for results
void printHeader() {
    printf("Elements, Runs, Algorithm, Variant, Elapsed time(ms), Message\n");
    if (resultsFile) fprintf(resultsFile, "Elements, Runs, Algorithm, Variant, Elapsed time(ms), Message\n");
}

// Writes a result row to file and console
void writeResultRow(unsigned long size, int count, const char* algorithmName, const char* variant, double elapsedMs) {
    char message[256];
    snprintf(message, sizeof(message), "%lu elements; %d runs - Algorithm: %s, Variant: %s, Elapsed time: %.6f ms", size, count, algorithmName, variant, elapsedMs);

    if (resultsFile) fprintf(resultsFile, "%lu, %d, %s, %s, %.3f, \"%s\"\n", size, count, algorithmName, variant, elapsedMs, message);
    printf("%s\n", message);
}

// Analyzes a sorting algorithm by running it on random, reversed, and partially sorted data
void analyzeSortAlg(int type, unsigned long size, int count) {
    const char* algoName = ALGORITHM_NAMES[type];

    int* masterRandom = generateIntArray(size);
    int* masterReversed = generateReverseSortedIntArray(size);
    int* working = malloc(sizeof(int) * size);

    clock_t start;
    double totalMs;

    start = clock();
    for (int i = 0; i < count; i++) {
        memcpy(working, masterRandom, size * sizeof(int));
        switch (type) {
            case BUBBLE_SORT: bubbleSort(working, size); break;
            case QUICK_SORT: quickSort(working, size); break;
            case INSERTION_SORT: insertionSort(working, size); break;
            case SELECTION_SORT: selectionSort(working, size); break;
            case MERGE_SORT: mergeSort(working, size); break;
            case RADIX_SORT: radixSort(working, size); break;
        }
    }
    totalMs = ((double)(clock() - start) * 1000.0) / CLOCKS_PER_SEC;
    writeResultRow(size, count, algoName, "random", totalMs / count);

    // --- REVERSED ---
    start = clock();
    for (int i = 0; i < count; i++) {
        memcpy(working, masterReversed, size * sizeof(int));
        switch (type) {
            case BUBBLE_SORT: bubbleSort(working, size); break;
            case QUICK_SORT: quickSort(working, size); break;
            case INSERTION_SORT: insertionSort(working, size); break;
            case SELECTION_SORT: selectionSort(working, size); break;
            case MERGE_SORT: mergeSort(working, size); break;
            case RADIX_SORT: radixSort(working, size); break;
        }
    }
    totalMs = ((double)(clock() - start) * 1000.0) / CLOCKS_PER_SEC;
    writeResultRow(size, count, algoName, "reversed", totalMs / count);

    // --- PARTIALLY SORTED (The new part) ---
    for (int factor = 10; factor <= 90; factor += 20) {
        int* masterPartial = generatePartiallySortedIntArray(size, factor);

        start = clock();
        for (int i = 0; i < count; i++) {
            memcpy(working, masterPartial, size * sizeof(int));
            switch (type) {
                case BUBBLE_SORT: bubbleSort(working, size); break;
                case QUICK_SORT: quickSort(working, size); break;
                case INSERTION_SORT: insertionSort(working, size); break;
                case SELECTION_SORT: selectionSort(working, size); break;
                case MERGE_SORT: mergeSort(working, size); break;
                case RADIX_SORT: radixSort(working, size); break;
            }
        }
        totalMs = ((double)(clock() - start) * 1000.0) / CLOCKS_PER_SEC;

        char varName[50];
        sprintf(varName, "partially sorted %d%%", factor);
        writeResultRow(size, count, algoName, varName, totalMs / count);

        free(masterPartial); // Free each partial array after its loop
    }

    // Clean up remaining memory
    free(masterRandom);
    free(masterReversed);
    free(working);
    printf("\n");
}

// Runs analysis for all sorting algorithms
void analyzeAlgo(unsigned long size, int count) {
     //analyzeSortAlg(BUBBLE_SORT, size, count);
     analyzeSortAlg(QUICK_SORT, size, count);
     //analyzeSortAlg(INSERTION_SORT, size, count);
     //analyzeSortAlg(SELECTION_SORT, size, count);
     //analyzeSortAlg(MERGE_SORT, size, count);
     //analyzeSortAlg(RADIX_SORT, size, count);
}

// Main function: sets up the benchmark and runs analyses for various array sizes
int main() {
    resetRandomSeed();
    resultsFile = fopen("results1.csv", "w");
    printHeader();

    //analyzeAlgo(10, 1000000);
    //analyzeAlgo(50, 1000000);
    //analyzeAlgo(100, 1000000);
    //analyzeAlgo(1000, 100);
    //analyzeAlgo(10000, 10);
    // analyzeAlgo(100000, 1);
    analyzeAlgo(1000000, 1);

    if (resultsFile) fclose(resultsFile);
    return 0;
}