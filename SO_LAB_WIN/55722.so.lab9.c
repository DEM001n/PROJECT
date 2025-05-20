// SO IS1 210B LAB9
// Dmytro Mahaliuk
// md55722@zut.edu.pl 

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// .\prog2.exe ...
// $LASTEXITCODE

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "brak argumentow");
        exit(201);
    }

    for (int i = 1; i < argc; i++)
    {
        int num = atoi(argv[i]);
        if (num < 0 || num > 100)
        {
            fprintf(stderr, "argument %d poza zakresem [0:100]", i);
            exit(202);
        }
    }

    if (argc == 2)
    {
        int i = atoi(argv[1]);
        exit(i);
    }
    else if (argc == 3)
    {
        int i = atoi(argv[1]);
        int j = atoi(argv[2]);
        int max = (i > j) ? i : j;
        exit(max);
    }

    int mid = (argc - 1) / 2 + 1;

    int length = strlen(argv[0]) + 1;
    for (int i = 1; i < mid; i++)
    {
        length += strlen(argv[i]);

        if (i < mid - 1)
            length += 1;
    }
    char *left_args = (char *)malloc(length * sizeof(char));

    sprintf(left_args, "%s", argv[0]);
    for (int i = 1; i < mid; i++)
    {
        strcat(left_args, " ");
        strcat(left_args, argv[i]);
    }

    length = strlen(argv[0]) + 1;
    for (int i = mid; i < argc; i++)
    {
        length += strlen(argv[i]);

        if (i < argc - 1)
            length += 1;
    }
    char *right_args = (char *)malloc(length * sizeof(char));

    sprintf(right_args, "%s", argv[0]);
    for (int i = mid; i < argc; i++)
    {
        strcat(right_args, " ");
        strcat(right_args, argv[i]);
    }

    STARTUPINFOA si1 = {0}, si2 = {0};

    PROCESS_INFORMATION pi1 = {0}, pi2 = {0};

    si1.cb = sizeof(si1);
    si2.cb = sizeof(si2);

    if (!CreateProcessA(NULL, left_args, NULL, NULL, FALSE, 0, NULL, NULL, &si1, &pi1))
    {

        free(left_args);
        free(right_args);
        fprintf(stderr, "Nie udalo sie stworzyc procesu lewego");
        exit(1);
    }

    if (!CreateProcessA(NULL, right_args, NULL, NULL, FALSE, 0, NULL, NULL, &si2, &pi2))
    {
        TerminateProcess(pi1.hProcess, 1);
        //WaitForSingleObject(pi1.hProcess,INFINITE);
        CloseHandle(pi1.hProcess);
        CloseHandle(pi1.hThread);
        free(left_args);
        free(right_args);
        fprintf(stderr, "Nie udalo sie stworzyc procesu prawego");
        exit(1);
    }

    HANDLE handles[2] = {pi1.hProcess, pi2.hProcess};
    WaitForMultipleObjects(2, handles, TRUE, INFINITE);

    DWORD exit_code1, exit_code2;
    GetExitCodeProcess(pi1.hProcess, &exit_code1);
    GetExitCodeProcess(pi2.hProcess, &exit_code2);

    printf("%d %d %d |", GetCurrentProcessId(), pi1.dwProcessId, exit_code1);
    for (int i = 1; i < mid; i++)
    {
        printf(" %s", argv[i]);
    }
    printf("\n");

    printf("%d %d %d |", GetCurrentProcessId(), pi2.dwProcessId, exit_code2);
    for (int i = mid; i < argc; i++)
    {
        printf(" %s", argv[i]);
    }
    printf("\n");

    int max_status = (exit_code1 > exit_code2) ? exit_code1 : exit_code2;
    printf("%d %d\n\n", GetCurrentProcessId(), max_status);

    CloseHandle(pi1.hProcess);
    CloseHandle(pi1.hThread);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi2.hThread);
    free(right_args);
    free(left_args);

    return max_status;
}
