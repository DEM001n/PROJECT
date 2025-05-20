// SO IS1 210B LAB13
// Dmytro Mahaliuk
// md55722@zut.edu.pl

// sender
// .\55722.so.lab13s.exe file255.bin _SHMEM_

// cmd /c fc [ /B | /L ] file255.bin b.bin

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <fcntl.h>
#include <io.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <file_name> <mapping_name>\n", argv[0]);
        return 1;
    }

    const char *file_name = argv[1];
    const char *mapping_name = argv[2];

    int fd = open(file_name, _O_RDONLY | _O_BINARY);
    if (fd == -1)
    {
        fprintf(stderr, "Failed to open file\n");
        return 1;
    }

    HANDLE hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 101, mapping_name);
    if (hMapping == NULL)
    {
        fprintf(stderr, "CreateFileMapping failed\n");
        close(fd);
        return 1;
    }

    char *shm_ptr = (char *)MapViewOfFile(hMapping, FILE_MAP_WRITE, 0, 0, 101);
    if (shm_ptr == NULL)
    {
        fprintf(stderr, "MapViewOfFile failed\n");
        close(fd);
        CloseHandle(hMapping);
        return 1;
    }

    printf("Waiting for receiver...\n");
    while (shm_ptr[0] != 103)
    {
        Sleep(100);
    }

    printf("Receiver connected. Sending file data...\n");
    DWORD bytes_read;
    while ((bytes_read = read(fd, shm_ptr + 1, 100)) > 0)
    {
        shm_ptr[0] = bytes_read;
        printf("%lu byte(s) sent...\n", bytes_read);

        while (shm_ptr[0] != 101)
        {
            Sleep(100);
        }
    }

    shm_ptr[0] = 102;

    printf("Copying finished\n");

    while (shm_ptr[0] != 104)
    {
        Sleep(100);
    }

    printf("Receiver finished\n");

    close(fd);
    UnmapViewOfFile(shm_ptr);
    CloseHandle(hMapping);

    printf("Shared memory detached\n");
    return 0;
}
