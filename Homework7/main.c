#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

int main(void) {
    int fd[2];
    pid_t pid1, pid2;

    if (pipe(fd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    pid1 = fork();
    if (pid1 < 0) {
        perror("fork for ls");
        return EXIT_FAILURE;
    }

    if (pid1 == 0) {
        // --- Tiến trình con 1: thực hiện "ls -l" ---
        // Chuyển stdout của tiến trình này tới đầu ghi của pipe
        if (dup2(fd[1], STDOUT_FILENO) == -1) {
            perror("dup2 ls");
            _exit(EXIT_FAILURE);
        }
        // Đóng cả hai đầu pipe ở tiến trình con (đã dup2 xong nên có thể đóng)
        close(fd[0]);
        close(fd[1]);

        // Thực thi lệnh ls -l
        execlp("ls", "ls", "-l", (char *)NULL);
        // Nếu execlp trả về, có lỗi
        fprintf(stderr, "execlp ls failed: %s\n", strerror(errno));
        _exit(EXIT_FAILURE);
    }

    // Tạo tiến trình con thứ hai
    pid2 = fork();
    if (pid2 < 0) {
        perror("fork for wc");
        return EXIT_FAILURE;
    }

    if (pid2 == 0) {
        // --- Tiến trình con 2: thực hiện "wc -l" ---
        // Chuyển stdin của tiến trình này tới đầu đọc của pipe
        if (dup2(fd[0], STDIN_FILENO) == -1) {
            perror("dup2 wc");
            _exit(EXIT_FAILURE);
        }
        // Đóng cả hai đầu pipe ở tiến trình con
        close(fd[0]);
        close(fd[1]);

        // Thực thi lệnh wc -l
        execlp("wc", "wc", "-l", (char *)NULL);
        // Nếu execlp trả về, có lỗi
        fprintf(stderr, "execlp wc failed: %s\n", strerror(errno));
        _exit(EXIT_FAILURE);
    }

    // --- Tiến trình cha ---
    // Cha không dùng tới pipe, phải đóng cả hai đầu để
    // các con biết khi nào stream kết thúc (EOF).
    close(fd[0]);
    close(fd[1]);

    // Chờ cả hai tiến trình con kết thúc
    int status;
    if (waitpid(pid1, &status, 0) == -1) {
        perror("waitpid pid1");
    }
    if (waitpid(pid2, &status, 0) == -1) {
        perror("waitpid pid2");
    }

    return EXIT_SUCCESS;
}
