#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_PATH "/tmp/unix_dgram_server"
#define CLIENT_PATH "/tmp/unix_dgram_client"

int main() {
    int sockfd;
    struct sockaddr_un servaddr, cliaddr;
    char buffer[1024];
    socklen_t len = sizeof(servaddr);

    unlink(CLIENT_PATH);

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);

    cliaddr.sun_family = AF_UNIX;
    strcpy(cliaddr.sun_path, CLIENT_PATH);
    bind(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, SERVER_PATH);

    sendto(sockfd, "Hello từ Unix Datagram Client!", 33, 0, (struct sockaddr *)&servaddr, len);
    recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
    printf("Server gửi: %s\n", buffer);

    close(sockfd);
    unlink(CLIENT_PATH);
    return 0;
}
