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
    socklen_t len = sizeof(cliaddr);

    unlink(SERVER_PATH);
    unlink(CLIENT_PATH);

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);

    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, SERVER_PATH);
    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len);
    printf("Client gửi: %s\n", buffer);

    char *msg = "Chào từ Unix Datagram Server!";
    sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&cliaddr, len);

    close(sockfd);
    unlink(SERVER_PATH);
    return 0;
}
