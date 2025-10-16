#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

struct product {
    int id;
    char name[50];
    float price;
};

int main() {
    const char *shm_name = "/my_shared_mem";
    const int SIZE = sizeof(struct product);

    // 1. Mở vùng nhớ chia sẻ đã được Producer tạo
    int shm_fd = shm_open(shm_name, O_RDONLY, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // 2. Ánh xạ vùng nhớ vào không gian tiến trình
    void *ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // 3. Đọc dữ liệu từ vùng nhớ chia sẻ
    struct product p;
    memcpy(&p, ptr, sizeof(struct product));

    printf("📦 Consumer: Đã đọc sản phẩm từ shared memory.\n");
    printf("   ID: %d\n   Tên: %s\n   Giá: %.2f\n", p.id, p.name, p.price);

    // 4. Giải phóng tài nguyên
    close(shm_fd);
    shm_unlink(shm_name);   // Xóa vùng nhớ chia sẻ sau khi xong

    return 0;
}
