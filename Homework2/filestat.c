#include <stdio.h>      // printf(), perror()
#include <stdlib.h>     // exit()
#include <sys/stat.h>   // struct stat, lstat(), S_ISREG()...
#include <unistd.h>     // needed for lstat()
#include <time.h>       // ctime(), strftime()

int main(int argc, char *argv[]) {
    // 1. Check command-line arguments
    if (argc != 2) {
        printf("Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    const char *path = argv[1];
    struct stat st;

    // 2. Use lstat() to get file information
    if (lstat(path, &st) == -1) {
        perror("lstat");
        return 1;
    }

    // 3. Print file path
    printf("File Path: %s\n", path);

    // 4. Determine file type
    if (S_ISREG(st.st_mode)) {
        printf("File Type: Regular File\n");
    } else if (S_ISDIR(st.st_mode)) {
        printf("File Type: Directory\n");
    } else if (S_ISLNK(st.st_mode)) {
        printf("File Type: Symbolic Link\n");
    } else {
        printf("File Type: Other\n");
    }

    // 5. Print file size
    printf("Size: %ld bytes\n", st.st_size);

    // 6. Print last modification time
    // Method 1: using ctime()
    printf("Last Modified (ctime): %s", ctime(&st.st_mtime));

    // Method 2: using strftime() for custom format
    char timebuf[80];
    struct tm *tm_info = localtime(&st.st_mtime);
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("Last Modified (strftime): %s\n", timebuf);

    return 0;
}

