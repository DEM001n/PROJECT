// SO IS1 210B LAB11
// Dmytro Mahaliuk
// md55722@zut.edu.pl 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>

// .\prog4 <num>
// $LASTEXITCODE

#define MAX_LINES 370104
#define ALPHABET_SIZE 26

char **words;
int global_histogram[ALPHABET_SIZE] = {0};
HANDLE mutex;

DWORD WINAPI calculate_histogram(LPVOID arg)
{
    int *args = (int *)arg;
    int local_histogram[ALPHABET_SIZE] = {0};

    for (int i = args[0]; i <= args[1]; i++)
    {
        for (char *ch = words[i]; *ch != '\0'; ch++)
        {
            local_histogram[*ch - 'a']++; // 'a' : 97, 'z' : 122
        }
    }

    WaitForSingleObject(mutex, INFINITE);
    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        global_histogram[i] += local_histogram[i];
    }
    ReleaseMutex(mutex);

    return GetCurrentThreadId();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);

    if (num_threads < 0 || num_threads > 16)
    {
        fprintf(stderr, "Number not in [0-16]\n");
        return 1;
    }

    words = malloc(sizeof(char *) * MAX_LINES);
    FILE *f = fopen("C:\\SO_LAB_WIN\\words_alpha.txt", "r");
    if (!f)
    {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    char line[64]; // Predicted word length (for entire line 255)

    for (int i = 0; i < MAX_LINES; i++)
    {
        if (fgets(line, sizeof(line), f))
        {
            line[strcspn(line, "\n")] = '\0';
            words[i] = _strdup(line);
        }
        else
        {
            break;
        }
    }
    fclose(f);

    mutex = CreateMutex(NULL, FALSE, NULL);
    if (!mutex)
    {
        fprintf(stderr, "Error creating mutex\n");
        return 1;
    }

    clock_t start_time = clock();

    int actual_threads = num_threads + 1;
    HANDLE *threads = malloc(sizeof(HANDLE) * actual_threads);
    int **args = malloc(sizeof(int *) * actual_threads);
    DWORD *thread_ids = malloc(sizeof(DWORD) * actual_threads);

    int words_per_thread = MAX_LINES / actual_threads;

    int start_idx = 0;
    int end_idx = words_per_thread - 1;

    args[0] = malloc(2 * sizeof(int));
    args[0][0] = start_idx;
    args[0][1] = end_idx;

    thread_ids[0] = calculate_histogram(args[0]);

    for (int i = 1; i < actual_threads; i++)
    {
        start_idx = i * words_per_thread;
        end_idx = (i + 1) * words_per_thread - 1;

        if (i == actual_threads - 1)
        {
            end_idx += MAX_LINES % actual_threads;
        }

        args[i] = malloc(2 * sizeof(int));
        args[i][0] = start_idx;
        args[i][1] = end_idx;

        threads[i] = CreateThread(NULL, 0, calculate_histogram, args[i], 0, &thread_ids[i]);
        if (!threads[i])
        {
            free(args);
            free(threads);
            free(words);
            free(thread_ids);
            CloseHandle(mutex);
            fprintf(stderr, "Error creating thread %d\n", i);
            return 1;
        }
    }

    for (int i = 0; i < actual_threads; i++)
    {
        WaitForSingleObject(threads[i], INFINITE);
        CloseHandle(threads[i]);
        free(args[i]);
    }

    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    for (int i = 1; i < actual_threads; i++)
    {
        printf("Thread #%lu started start=%d, end=%d\n", thread_ids[i], args[i][0], args[i][1]);
        printf("Thread #%lu stopped\n", thread_ids[i]);
    }

    printf("\nExecution time: %.6f s\n", elapsed_time);

    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        if (i % 5 == 0)
        {
            printf("\n");
        }
        printf("%c : %6d  ", 'a' + i, global_histogram[i]);
    }
    printf("\n\n");

    for (int i = 0; i < MAX_LINES; i++)
    {
        free(words[i]);
    }
    free(args);
    free(threads);
    free(words);
    free(thread_ids);
    CloseHandle(mutex);

    return 0;
}
