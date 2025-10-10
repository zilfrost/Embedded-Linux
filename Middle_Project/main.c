#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ifaddrs.h>
#include <netdb.h>

#define BUFFER_SIZE 1024
#define MAX_PEERS 100

typedef struct {
    char ip[INET_ADDRSTRLEN];
    int port;
    int fd;
} Peer;

Peer peers[MAX_PEERS];
int peer_count = 0;
int listen_sock;

// 🧠 Hàm lấy IP thật (bỏ qua 127.0.0.1)
void get_local_ip(char *ip_str, size_t size) {
    struct ifaddrs *ifaddr, *ifa;
    getifaddrs(&ifaddr);
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            inet_ntop(AF_INET, &sa->sin_addr, ip_str, size);
            if (strcmp(ip_str, "127.0.0.1") != 0) break; // bỏ qua localhost
        }
    }
    freeifaddrs(ifaddr);
}

// 📨 Thread nhận tin nhắn
void *receive_thread(void *arg) {
    int fd = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    char sender_ip[INET_ADDRSTRLEN];
    int sender_port = 0;

    // Lấy thông tin địa chỉ IP và port của peer gửi
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    getpeername(fd, (struct sockaddr *)&addr, &addr_len);
    inet_ntop(AF_INET, &addr.sin_addr, sender_ip, sizeof(sender_ip));
    sender_port = ntohs(addr.sin_port);

    while (1) {
        int n = recv(fd, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) break;
        buffer[n] = '\0';
        printf("\n📩 Tin nhắn từ %s:%d → %s\n> ", sender_ip, sender_port, buffer);
        fflush(stdout);
    }

    // Xoá peer khi ngắt kết nối
    for (int i = 0; i < peer_count; i++) {
        if (peers[i].fd == fd) {
            printf("\n⚠️  Peer %s:%d đã ngắt kết nối.\n> ", peers[i].ip, peers[i].port);
            peers[i] = peers[peer_count - 1];
            peer_count--;
            break;
        }
    }
    close(fd);
    return NULL;
}

// 🧑‍💻 Thread server lắng nghe
void *server_thread(void *arg) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (1) {
        int conn_fd = accept(listen_sock, (struct sockaddr *)&client_addr, &addr_len);
        if (conn_fd < 0) continue;

        if (peer_count < MAX_PEERS) {
            Peer p;
            inet_ntop(AF_INET, &client_addr.sin_addr, p.ip, sizeof(p.ip));
            p.port = ntohs(client_addr.sin_port);
            p.fd = conn_fd;
            peers[peer_count++] = p;

            printf("\n📡 Máy %s:%d đã kết nối tới bạn\n> ", p.ip, p.port);
            fflush(stdout);

            pthread_t recv_tid;
            int *fd_ptr = malloc(sizeof(int));
            *fd_ptr = conn_fd;
            pthread_create(&recv_tid, NULL, receive_thread, fd_ptr);
            pthread_detach(recv_tid);
        } else {
            close(conn_fd);
        }
    }
    return NULL;
}

// 📡 Hàm kết nối tới peer khác
void connect_to_peer(const char *ip, int port) {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in peer_addr;
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &peer_addr.sin_addr);

    if (connect(sock_fd, (struct sockaddr *)&peer_addr, sizeof(peer_addr)) == 0) {
        Peer p;
        strcpy(p.ip, ip);
        p.port = port;
        p.fd = sock_fd;
        peers[peer_count++] = p;

        printf("✅ Đã kết nối tới %s:%d\n> ", ip, port);

        pthread_t recv_tid;
        int *fd_ptr = malloc(sizeof(int));
        *fd_ptr = sock_fd;
        pthread_create(&recv_tid, NULL, receive_thread, fd_ptr);
        pthread_detach(recv_tid);
    } else {
        perror("❌ Kết nối thất bại");
        close(sock_fd);
    }
}

// 📝 Hiển thị danh sách peer
void list_peers() {
    printf("📜 Danh sách các kết nối:\n");
    for (int i = 0; i < peer_count; i++) {
        printf(" - %s:%d\n", peers[i].ip, peers[i].port);
    }
    if (peer_count == 0) printf(" (Không có kết nối nào)\n");
}

// 🔌 Ngắt kết nối 1 peer (terminate)
void terminate_peer(const char *ip, int port) {
    for (int i = 0; i < peer_count; i++) {
        if (strcmp(peers[i].ip, ip) == 0 && peers[i].port == port) {
            close(peers[i].fd);
            peers[i] = peers[peer_count - 1];
            peer_count--;
            printf("❌ Đã ngắt kết nối %s:%d\n> ", ip, port);
            return;
        }
    }
    printf("⚠️ Không tìm thấy peer %s:%d\n> ", ip, port);
}

// 📴 Thoát toàn bộ
void exit_all() {
    for (int i = 0; i < peer_count; i++) {
        close(peers[i].fd);
    }
    close(listen_sock);
    printf("👋 Đã thoát chương trình.\n");
    exit(0);
}

// 🆘 Lệnh help
void show_help() {
    printf("\n📖 Các lệnh hỗ trợ:\n");
    printf("  connect <IP> <PORT>        - Kết nối tới peer khác\n");
    printf("  send <IP> <PORT> <MSG>     - Gửi tin nhắn tới peer\n");
    printf("  list                       - Liệt kê các peer đã kết nối\n");
    printf("  terminate <IP> <PORT>      - Ngắt kết nối tới peer\n");
    printf("  exit                       - Ngắt toàn bộ kết nối và thoát\n");
    printf("  help                       - Hiển thị bảng hướng dẫn\n\n");
}

// 📬 Gửi tin nhắn
void send_message(const char *ip, int port, const char *msg) {
    for (int i = 0; i < peer_count; i++) {
        if (strcmp(peers[i].ip, ip) == 0 && peers[i].port == port) {
            send(peers[i].fd, msg, strlen(msg), 0);
            return;
        }
    }
    printf("⚠️ Peer %s:%d không tồn tại\n> ", ip, port);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Cách dùng: %s <PORT>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    bind(listen_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(listen_sock, 5);

    char local_ip[INET_ADDRSTRLEN];
    get_local_ip(local_ip, sizeof(local_ip));

    printf("🚀 Đã khởi tạo %s %d\n", local_ip, port);
    printf("👉 Dùng \"help\" để hiển thị các lệnh\n");

    pthread_t server_tid;
    pthread_create(&server_tid, NULL, server_thread, NULL);
    pthread_detach(server_tid);

    char cmd[BUFFER_SIZE];
    while (1) {
        printf("> ");
        fflush(stdout);
        if (!fgets(cmd, sizeof(cmd), stdin)) break;

        char *args[4];
        int n = 0;
        char *token = strtok(cmd, " \n");
        while (token && n < 4) {
            args[n++] = token;
            token = strtok(NULL, " \n");
        }

        if (n == 0) continue;

        if (strcmp(args[0], "connect") == 0 && n == 3) {
            connect_to_peer(args[1], atoi(args[2]));
        } else if (strcmp(args[0], "send") == 0 && n >= 4) {
            char msg[BUFFER_SIZE] = "";
            for (int i = 3; i < n; i++) {
                strcat(msg, args[i]);
                if (i < n - 1) strcat(msg, " ");
            }
            send_message(args[1], atoi(args[2]), msg);
        } else if (strcmp(args[0], "list") == 0) {
            list_peers();
        } else if (strcmp(args[0], "terminate") == 0 && n == 3) {
            terminate_peer(args[1], atoi(args[2]));
        } else if (strcmp(args[0], "exit") == 0) {
            exit_all();
        } else if (strcmp(args[0], "help") == 0) {
            show_help();
        } else {
            printf("⚠️ Lệnh không hợp lệ. Gõ \"help\" để xem hướng dẫn.\n");
        }
    }

    return 0;
}
