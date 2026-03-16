#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

void bubble_sort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j + 1]){
                int t = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = t;
            }
}

void insertion_sort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i], j = i - 1;
        while (j >= 0 && arr[j] > key){
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void merge_logic(int arr[], int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    int *L = malloc(n1 * sizeof(int)), *R = malloc(n2 * sizeof(int));
    for (int i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2)
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    while (i < n1)
        arr[k++] = L[i++];
    while (j < n2)
        arr[k++] = R[j++];
    free(L);
    free(R);
}

void merge_sort(int arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        merge_sort(arr, l, m);
        merge_sort(arr, m + 1, r);
        merge_logic(arr, l, m, r);
    }
}

void quick_sort(int arr[], int low, int high) {
    int i = low, j = high, pivot = arr[low + (high - low) / 2];
    while (i <= j) {
        while (arr[i] < pivot)
            i++;
        while (arr[j] > pivot)
            j--;
        if (i <= j){
            int t = arr[i];
            arr[i] = arr[j];
            arr[j] = t;
            i++;
            j--;
        }
    }
    if (low < j)
        quick_sort(arr, low, j);
    if (i < high)
        quick_sort(arr, i, high);
}

void radix_sort(int arr[], int n) {
    if (n < 2)
        return;
    int max = arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] > max)
            max = arr[i];
    int *output = malloc(n * sizeof(int));
    for (int exp = 1; max / exp > 0; exp *= 10) {
        int count[10] = {0};
        for (int i = 0; i < n; i++)
            count[(arr[i] / exp) % 10]++;
        for (int i = 1; i < 10; i++)
            count[i] += count[i - 1];
        for (int i = n - 1; i >= 0; i--){
            output[count[(arr[i] / exp) % 10] - 1] = arr[i];
            count[(arr[i] / exp) % 10]--;
        }
        for (int i = 0; i < n; i++)
            arr[i] = output[i];
    }
    free(output);
}

Node* sorted_merge(Node* a, Node* b) {
    if (!a)
        return b;
    if (!b)
        return a;
    Node* res;
    if (a->data <= b->data){
        res = a; res->next = sorted_merge(a->next, b);
    } else {
        res = b;
        res->next = sorted_merge(a, b->next);
    }
    return res;
}

void split_list(Node* source, Node** front, Node** back) {
    Node *fast = source->next, *slow = source;
    while (fast != NULL){
        fast = fast->next;
        if (fast != NULL){
            slow = slow->next;
            fast = fast->next;
        }
    }
    *front = source;
    *back = slow->next;
    slow->next = NULL;
}

void list_merge_sort(Node** headRef) {
    Node* head = *headRef;
    if (!head || !head->next)
        return;
    Node *a, *b;
    split_list(head, &a, &b);
    list_merge_sort(&a);
    list_merge_sort(&b);
    *headRef = sorted_merge(a, b);
}

void m_wrap(int a[], int n){
    merge_sort(a, 0, n - 1);
}
void q_wrap(int a[], int n){
    quick_sort(a, 0, n - 1);
}


void generate_data(int arr[], int n, const char* type) {
    if (strcmp(type, "Random") == 0)
        for(int i=0; i<n; i++)
            arr[i] = rand();
    else if (strcmp(type, "Sorted") == 0)
        for(int i=0; i<n; i++)
            arr[i] = i;
    else if (strcmp(type, "Reversed") == 0)
        for(int i=0; i<n; i++)
            arr[i] = n - i;
    else if (strcmp(type, "Almost") == 0){
        for(int i=0; i<n; i++)
            arr[i] = i;
        for(int i=0; i<n/50; i++)
            arr[rand()%n] = rand();
    }
    else if (strcmp(type, "Flat") == 0)
        for(int i=0; i<n; i++)
            arr[i] = rand() % 10;
}

void execute_test(int n, const char* alg_name, void (*func)(int*, int), const char* struct_type) {
    if (n >= 100000 && (strcmp(alg_name, "Bubble") == 0 || strcmp(alg_name, "Insertion") == 0)) {
        printf("%-10s | %-12s | %-10d | SKIPPED: O(n^2) too slow for large N.\n", alg_name, struct_type, n);
        return;
    }

    int *work = malloc(n * sizeof(int));
    int runs = (n <= 1000) ? 10000 : 1;
    double total_time = 0;

    for (int r = 0; r < runs; r++) {
        generate_data(work, n, struct_type);
        clock_t start = clock();
        func(work, n);
        total_time += (double)(clock() - start) / CLOCKS_PER_SEC;
    }

    printf("%-10s | %-12s | %-10d | Time: %.8f s\n", alg_name, struct_type, n, total_time / runs);
    free(work);
}

int main() {
    srand(time(NULL));
    int sizes[] = {20, 50, 100, 1000, 10000, 100000, 1000000};
    const char* names[] = {"Bubble", "Insertion", "Merge", "Quick", "Radix"};
    void (*funcs[])(int*, int) = {bubble_sort, insertion_sort, m_wrap, q_wrap, radix_sort};
    const char* structs[] = {"Random", "Sorted", "Reversed", "Almost", "Flat"};

    printf("%-10s | %-12s | %-10s | %s\n", "ALGO", "STRUCTURE", "SIZE", "RESULT");
    printf("--------------------------------------------------------------------------\n");
    for (int s = 0; s < 7; s++) {
        for (int a = 0; a < 5; a++) {
            for (int st = 0; st < 5; st++) {
                execute_test(sizes[s], names[a], funcs[a], structs[st]);
            }
        }
        printf("--------------------------------------------------------------------------\n");
    }

    return 0;
}