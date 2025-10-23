#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <ifaddrs.h>
#include <netdb.h>

#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

typedef struct {
    int sock;
    char ip[INET_ADDRSTRLEN];
    int port;
    char local_ip[INET_ADDRSTRLEN];
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
void add_peer(int sock, const char *ip, int port, const char *local_ip_conn) {
    if (peer_count >= MAX_CLIENTS) return;
    peers[peer_count].sock = sock;
    strcpy(peers[peer_count].ip, ip);
    peers[peer_count].port = port;
    strcpy(peers[peer_count].local_ip, local_ip_conn);
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
            printf("[-] Kết nối Remote %s:%d đã bị ngắt.\n", 
                   peer->ip, peer->port);
            remove_peer(peer->ip, peer->port);
            break;
        }

        buffer[bytes] = '\0';

        if (strcmp(buffer, "__TERMINATE__") == 0) {
            printf("[!] Remote %s:%d đã ngắt kết nối.\n", 
                   peer->ip, peer->port);
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
    if (sock < 0) {
        perror("socket client");
        return;
    }

    // *** THAY ĐỔI: SET CẢ REUSEADDR VÀ REUSEPORT ***
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt(SO_REUSEADDR) for client");
        close(sock);
        return;
    }
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt(SO_REUSEPORT) for client");
        close(sock);
        return;
    }
    // *** KẾT THÚC THAY ĐỔI ***

    // Bind vào INADDR_ANY
    struct sockaddr_in local_bind_addr;
    memset(&local_bind_addr, 0, sizeof(local_bind_addr));
    local_bind_addr.sin_family = AF_INET;
    local_bind_addr.sin_port = htons(server_port); 
    local_bind_addr.sin_addr.s_addr = INADDR_ANY; // Bind vào 0.0.0.0

    if (bind(sock, (struct sockaddr *)&local_bind_addr, sizeof(local_bind_addr)) < 0) {
        perror("bind client socket"); 
        close(sock);
        return;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return;
    }
    
    // Lấy IP local mà KERNEL đã chọn
    struct sockaddr_in actual_local_addr;
    socklen_t len = sizeof(actual_local_addr);
    char actual_local_ip[INET_ADDRSTRLEN];
    
    if (getsockname(sock, (struct sockaddr *)&actual_local_addr, &len) == 0) {
        inet_ntop(AF_INET, &actual_local_addr.sin_addr, actual_local_ip, sizeof(actual_local_ip));
    } else {
        perror("getsockname after connect");
        strcpy(actual_local_ip, "unknown"); 
    }

    add_peer(sock, ip, port, actual_local_ip); 
    
    printf("[+] Đã kết nối tới %s:%d\n", 
           ip, port);

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

        struct sockaddr_in local_conn_addr;
        socklen_t local_conn_len = sizeof(local_conn_addr);
        char local_conn_ip[INET_ADDRSTRLEN];
        int local_conn_port = 0;

        if (getsockname(client_sock, (struct sockaddr *)&local_conn_addr, &local_conn_len) == 0) {
            inet_ntop(AF_INET, &local_conn_addr.sin_addr, local_conn_ip, sizeof(local_conn_ip));
            local_conn_port = ntohs(local_conn_addr.sin_port);
        } else {
            strcpy(local_conn_ip, local_ip); 
            local_conn_port = server_port;
        }
        
        add_peer(client_sock, ip, port, local_conn_ip);
        printf("[+] Kết nối mới từ %s:%d\n", 
               ip, port);

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
            
            printf("[x] Đã ngắt kết nối với %s:%d\n", 
                   peers[i].ip, peers[i].port);
            
            remove_peer(ip, port);
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
        printf(" - Remote: %s:%d\n", 
               peers[i].ip, peers[i].port);
    }
}

// =========================
// HELP
// =========================
void print_help() {
    printf("Các lệnh hỗ trợ:\n");
    printf("  connect <ip> <port>      : Kết nối tới một peer khác.\n");
    printf("                           (Dùng IP LAN/Public cho máy khác)\n");
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
    if (server_sock < 0) {
        perror("socket server");
        return 1;
    }

    // *** THAY ĐỔI: SET CẢ REUSEADDR VÀ REUSEPORT ***
    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt(SO_REUSEADDR) for server");
        close(server_sock);
        return 1;
    }
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt(SO_REUSEPORT) for server");
        close(server_sock);
        return 1;
    }
    // *** KẾT THÚC THAY ĐỔI ***


    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    
    // *** THAY ĐỔI: QUAY LẠI BIND VÀO INADDR_ANY ***
    server_addr.sin_addr.s_addr = INADDR_ANY; // Server bind vào 0.0.0.0
    // *** KẾT THÚC THAY ĐỔI ***

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind server");
        close(server_sock);
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