#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>      // O_CREAT, O_RDWR
#include <sys/mman.h>   // shm_open, mmap
#include <sys/stat.h>   // S_IRUSR, S_IWUSR
#include <unistd.h>     // ftruncate, close
#include <string.h>     // strcpy

// Cấu trúc dữ liệu lưu thông tin sản phẩm
struct product {
    int id;
    char name[50];
    float price;
};

int main() {
    const char *shm_name = "/my_shared_mem";   // Tên vùng nhớ chia sẻ
    const int SIZE = sizeof(struct product);   // Kích thước vùng nhớ

    // 1. Tạo (hoặc mở nếu có sẵn) vùng nhớ chia sẻ
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // 2. Đặt kích thước vùng nhớ chia sẻ
    if (ftruncate(shm_fd, SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // 3. Ánh xạ vùng nhớ vào không gian tiến trình
    void *ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // 4. Ghi thông tin sản phẩm vào bộ nhớ chia sẻ
    struct product p;
    p.id = 101;
    strcpy(p.name, "STM32F103C8T6");
    p.price = 3.5;

    memcpy(ptr, &p, sizeof(struct product));
    printf("✅ Producer: Đã ghi sản phẩm vào shared memory.\n");
    printf("   ID: %d\n   Tên: %s\n   Giá: %.2f\n", p.id, p.name, p.price);

    // 5. Đóng file descriptor (vùng nhớ vẫn tồn tại)
    close(shm_fd);

    return 0;
}
