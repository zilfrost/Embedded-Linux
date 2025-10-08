#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/unix_stream_socket"

int main() {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buffer[1024] = {0};

    unlink(SOCKET_PATH);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 5);

    printf("Unix Stream Server đang chờ kết nối...\n");
    client_fd = accept(server_fd, NULL, NULL);

    read(client_fd, buffer, sizeof(buffer));
    printf("Client gửi: %s\n", buffer);

    char *msg = "Chào từ Unix Stream Server!";
    write(client_fd, msg, strlen(msg));

    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
