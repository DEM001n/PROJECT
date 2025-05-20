// SO IS1 210B LAB13
// Dmytro Mahaliuk
// md55722@zut.edu.pl

//receiver
// .\55722.so.lab13r.exe b.bin _SHMEM_

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h> 


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <output_file> <mapping_name>\n", argv[0]);
        return 1;
    }

    const char *output_file = argv[1];
    const char *mapping_name = argv[2];

    HANDLE hMapping = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, mapping_name);
    if (hMapping == NULL) {
        fprintf(stderr, "OpenFileMapping failed\n");
        return 1;
    }

    char *shm_ptr = (char *)MapViewOfFile(hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 101);
    if (shm_ptr == NULL) {
        fprintf(stderr, "MapViewOfFile failed\n");
        CloseHandle(hMapping);
        return 1;
    }

    int fd = open(output_file, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IREAD | _S_IWRITE);
    if (fd == -1) {
        fprintf(stderr, "Failed to open output file\n");
        UnmapViewOfFile(shm_ptr);
        CloseHandle(hMapping);
        return 1;
    }

    shm_ptr[0] = 103;
    printf("Shared memory attached, ready to receive\n");

    while (1) {
        while (shm_ptr[0] != 102 && shm_ptr[0] > 100) {
            Sleep(100);
        }

        unsigned char data_size = shm_ptr[0];
        if (data_size == 102)
            break;

        write(fd, shm_ptr + 1, data_size);
        printf("%d byte(s) received...\n",data_size);

        shm_ptr[0] = 101;
    }

    printf("Copying finished\n");
    close(fd);

    shm_ptr[0] = 104;
    UnmapViewOfFile(shm_ptr);
    CloseHandle(hMapping);

    printf("Shared memory detached\n");
    return 0;
}
