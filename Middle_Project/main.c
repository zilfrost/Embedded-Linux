#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <ifaddrs.h>
#include <netdb.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

typedef struct {
    int sock;
    char ip[INET_ADDRSTRLEN];
    int port;
} Peer;

Peer peers[MAX_CLIENTS];
int peer_count = 0;
int server_port;
char local_ip[INET_ADDRSTRLEN];

// =========================
// Lấy IP máy cục bộ
// =========================
void get_local_ip(char *ip_buffer) {
    struct ifaddrs *ifaddr, *ifa;
    int family;
    ip_buffer[0] = '\0';

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        strcpy(ip_buffer, "127.0.0.1");
        return;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        family = ifa->ifa_addr->sa_family;
        if (family == AF_INET && strcmp(ifa->ifa_name, "lo") != 0) {
            getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                        ip_buffer, INET_ADDRSTRLEN,
                        NULL, 0, NI_NUMERICHOST);
            break;
        }
    }

    freeifaddrs(ifaddr);
    if (strlen(ip_buffer) == 0) strcpy(ip_buffer, "127.0.0.1");
}

// =========================
// Thêm peer
// =========================
void add_peer(int sock, const char *ip, int port) {
    if (peer_count >= MAX_CLIENTS) return;
    peers[peer_count].sock = sock;
    strcpy(peers[peer_count].ip, ip);
    peers[peer_count].port = port;
    peer_count++;
}

// =========================
// Xóa peer
// =========================
void remove_peer(const char *ip, int port) {
    for (int i = 0; i < peer_count; i++) {
        if (strcmp(peers[i].ip, ip) == 0 && peers[i].port == port) {
            close(peers[i].sock);
            for (int j = i; j < peer_count - 1; j++) {
                peers[j] = peers[j + 1];
            }
            peer_count--;
            break;
        }
    }
}

// =========================
// Gửi tin nhắn
// =========================
void send_message(const char *ip, int port, const char *msg) {
    for (int i = 0; i < peer_count; i++) {
        if (strcmp(peers[i].ip, ip) == 0 && peers[i].port == port) {
            send(peers[i].sock, msg, strlen(msg), 0);
            return;
        }
    }
    printf("[!] Không tìm thấy kết nối tới %s:%d\n", ip, port);
}

// =========================
// Xử lý nhận tin nhắn
// =========================
void *recv_thread(void *arg) {
    Peer *peer = (Peer *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        ssize_t bytes = recv(peer->sock, buffer, sizeof(buffer)-1, 0);
        if (bytes <= 0) {
            printf("[-] Kết nối tới %s:%d đã bị ngắt.\n", peer->ip, peer->port);
            remove_peer(peer->ip, peer->port);
            break;
        }

        buffer[bytes] = '\0';

        // Nếu nhận lệnh terminate
        if (strcmp(buffer, "__TERMINATE__") == 0) {
            printf("[!] %s:%d đã ngắt kết nối.\n", peer->ip, peer->port);
            remove_peer(peer->ip, peer->port);
            break;
        }

        printf("[%s:%d] %s\n", peer->ip, peer->port, buffer);
    }

    return NULL;
}

// =========================
// Kết nối tới peer khác
// =========================
void connect_to_peer(const char *ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return;
    }

    struct sockaddr_in local_addr;
    socklen_t len = sizeof(local_addr);
    getsockname(sock, (struct sockaddr *)&local_addr, &len);

    char my_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &local_addr.sin_addr, my_ip, sizeof(my_ip));
    int my_port = ntohs(local_addr.sin_port);

    add_peer(sock, ip, port);
    printf("[+] Đã kết nối tới %s:%d (local port %d)\n", ip, port, my_port);

    pthread_t tid;
    Peer *peer = malloc(sizeof(Peer));
    *peer = peers[peer_count-1];
    pthread_create(&tid, NULL, recv_thread, peer);
    pthread_detach(tid);
}

// =========================
// Thread lắng nghe kết nối
// =========================
void *server_thread(void *arg) {
    int server_sock = *(int *)arg;
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);

    while (1) {
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sock < 0) continue;

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
        int port = ntohs(client_addr.sin_port);

        add_peer(client_sock, ip, port);
        printf("[+] Kết nối mới từ %s:%d\n", ip, port);

        pthread_t tid;
        Peer *peer = malloc(sizeof(Peer));
        *peer = peers[peer_count-1];
        pthread_create(&tid, NULL, recv_thread, peer);
        pthread_detach(tid);
    }

    return NULL;
}

// =========================
// Lệnh terminate
// =========================
void terminate_connection(const char *ip, int port) {
    for (int i = 0; i < peer_count; i++) {
        if (strcmp(peers[i].ip, ip) == 0 && peers[i].port == port) {
            send(peers[i].sock, "__TERMINATE__", strlen("__TERMINATE__"), 0);
            close(peers[i].sock);
            remove_peer(ip, port);
            printf("[x] Đã ngắt kết nối với %s:%d\n", ip, port);
            return;
        }
    }
    printf("[!] Không tìm thấy kết nối tới %s:%d\n", ip, port);
}

// =========================
// Hiển thị danh sách kết nối
// =========================
void list_connections() {
    printf("Các kết nối hiện tại (%d):\n", peer_count);
    for (int i = 0; i < peer_count; i++) {
        printf(" - %s:%d\n", peers[i].ip, peers[i].port);
    }
}

// =========================
// HELP
// =========================
void print_help() {
    printf("Các lệnh hỗ trợ:\n");
    printf("  connect <ip> <port>      : Kết nối tới một peer khác\n");
    printf("  send <ip> <port> <msg>   : Gửi tin nhắn tới peer\n");
    printf("  terminate <ip> <port>    : Ngắt kết nối với peer\n");
    printf("  list                     : Liệt kê các kết nối hiện tại\n");
    printf("  exit                     : Ngắt toàn bộ kết nối và thoát chương trình\n");
    printf("  help                     : Hiển thị hướng dẫn\n");
}

// =========================
// Main
// =========================
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Cách dùng: %s <port>\n", argv[0]);
        return 1;
    }

    server_port = atoi(argv[1]);
    get_local_ip(local_ip);

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        return 1;
    }

    listen(server_sock, 5);

    pthread_t server_tid;
    pthread_create(&server_tid, NULL, server_thread, &server_sock);
    pthread_detach(server_tid);

    printf("[*] Đã khởi tạo %s:%d\n", local_ip, server_port);
    printf("[*] Dùng \"help\" để hiển thị các lệnh\n");

    char command[BUFFER_SIZE];
    while (1) {
        printf("> ");
        fflush(stdout);
        if (!fgets(command, sizeof(command), stdin)) break;

        command[strcspn(command, "\n")] = 0;
        if (strncmp(command, "connect", 7) == 0) {
            char ip[64]; int port;
            if (sscanf(command, "connect %63s %d", ip, &port) == 2)
                connect_to_peer(ip, port);
        }
        else if (strncmp(command, "send", 4) == 0) {
            char ip[64]; int port; char msg[BUFFER_SIZE];
            if (sscanf(command, "send %63s %d %[^\n]", ip, &port, msg) == 3)
                send_message(ip, port, msg);
        }
        else if (strncmp(command, "terminate", 9) == 0) {
            char ip[64]; int port;
            if (sscanf(command, "terminate %63s %d", ip, &port) == 2)
                terminate_connection(ip, port);
        }
        else if (strcmp(command, "list") == 0) {
            list_connections();
        }
        else if (strcmp(command, "exit") == 0) {
            for (int i = 0; i < peer_count; i++) {
                send(peers[i].sock, "__TERMINATE__", strlen("__TERMINATE__"), 0);
                close(peers[i].sock);
            }
            peer_count = 0;
            printf("[x] Đã ngắt toàn bộ kết nối. Thoát chương trình.\n");
            break;
        }
        else if (strcmp(command, "help") == 0) {
            print_help();
        }
        else if (strlen(command) > 0) {
            printf("[!] Lệnh không hợp lệ. Gõ \"help\" để xem hướng dẫn.\n");
        }
    }

    close(server_sock);
    return 0;
}
