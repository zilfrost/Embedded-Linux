#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/unix_stream_socket"

int main() {
    int sockfd;
    struct sockaddr_un addr;
    char buffer[1024] = {0};

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));

    write(sockfd, "Hello từ Unix Stream Client!", 30);
    read(sockfd, buffer, sizeof(buffer));
    printf("Server gửi: %s\n", buffer);

    close(sockfd);
    return 0;
}
