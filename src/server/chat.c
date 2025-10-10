// chat:.c
// Build: gcc -O2 -Wall -Wextra -o server server.c
// Usage: ./server <listen_port> <peer_host> <peer_port> <node_name>
#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_CLIENTS 128
#define MAX_LINE 2048
#define MAX_ROOM 64
#define MAX_USER 64

typedef struct {
    int fd;
    char buf[MAX_LINE];
    size_t len;
    char user[MAX_USER];
    char room[MAX_ROOM];
    bool authed;
} Client;

static Client clients[MAX_CLIENTS];
static int srv_fd = -1;
static int peer_fd = -1;
static char NODE[32] = "N";
static char PEER_HOST[128];
static int PEER_PORT = 0;
static time_t last_heartbeat = 0;
static volatile sig_atomic_t running = 1;

static void die(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    vfprintf(stderr, fmt, ap); fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}

static int nonblock(int fd) {
    // Keep it simple—use blocking sockets; select() prevents blocking reads/writes.
    (void)fd; return 0;
}

static int listen_on(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) die("socket: %s", strerror(errno));
    int yes = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    struct sockaddr_in a = {0};
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = INADDR_ANY;
    a.sin_port = htons(port);
    if (bind(fd, (struct sockaddr*)&a, sizeof(a)) < 0) die("bind: %s", strerror(errno));
    if (listen(fd, 128) < 0) die("listen: %s", strerror(errno));
    return fd;
}

static int connect_to_peer(const char *host, int port) {
    char pstr[16]; snprintf(pstr, sizeof pstr, "%d", port);
    struct addrinfo hints = {0}, *res = NULL;
    hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, pstr, &hints, &res) != 0) return -1;
    int fd = -1;
    for (struct addrinfo *it = res; it; it = it->ai_next) {
        fd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (fd < 0) continue;
        if (connect(fd, it->ai_addr, it->ai_addrlen) == 0) break;
        close(fd); fd = -1;
    }
    freeaddrinfo(res);
    return fd;
}

static void remove_client(int i) {
    if (clients[i].fd != -1) close(clients[i].fd);
    clients[i].fd = -1; clients[i].len = 0; clients[i].authed = false;
    clients[i].user[0]=0; clients[i].room[0]=0; clients[i].buf[0]=0;
}

static void broadcast_room(const char *room, const char *line) {
    for (int i=0;i<MAX_CLIENTS;i++) {
        if (clients[i].fd != -1 && clients[i].authed && strcmp(clients[i].room, room)==0) {
            send(clients[i].fd, line, strlen(line), 0);
        }
    }
}

static void send_fmt(int fd, const char *fmt, ...) {
    char out[MAX_LINE];
    va_list ap; va_start(ap, fmt);
    vsnprintf(out, sizeof out, fmt, ap);
    va_end(ap);
    send(fd, out, strlen(out), 0);
}

static void handle_line_client(int i, char *line);

static void try_peer_connect() {
    if (peer_fd != -1) return;
    peer_fd = connect_to_peer(PEER_HOST, PEER_PORT);
    if (peer_fd != -1) {
        fprintf(stderr, "[%s] connected to peer %s:%d\n", NODE, PEER_HOST, PEER_PORT);
        send_fmt(peer_fd, "PEER_HELLO %s\n", NODE);
    }
}

static void relay_to_peer(const char *room, const char *from, const char *msg) {
    if (peer_fd == -1) return;
    // msgid is ts + pid + rand (quick & dirty)
    char msgid[64];
    struct timeval tv; gettimeofday(&tv,NULL);
    snprintf(msgid, sizeof msgid, "%ld.%06ld.%d", (long)tv.tv_sec, (long)tv.tv_usec, getpid());
    send_fmt(peer_fd, "RELAY %s %s %s %s\n", room, from, msgid, msg);
}

static void handle_line_peer(char *line) {
    // Formats:
    // RELAY <room> <from> <msgid> <text...>
    // HEARTBEAT <ts>
    char cmd[16];
    if (sscanf(line, "%15s", cmd) != 1) return;

    if (strcmp(cmd,"RELAY")==0) {
        // Find fields after the command
        // We’ll parse token by token: RELAY room from msgid text...
        char *p = line + 6; // skip "RELAY "
        char *room = strtok(p, " \t\r\n");
        char *from = strtok(NULL, " \t\r\n");
        char *msgid= strtok(NULL, " \t\r\n");
        char *rest = strtok(NULL, "\n");
        if (!room || !from || !msgid || !rest) return;
        // Broadcast locally, do NOT forward back to peer (prevents loop in 2-node setup)
        char out[MAX_LINE];
        snprintf(out, sizeof out, "EVENT %s %s %s\n", room, from, rest);
        broadcast_room(room, out);
    } else if (strcmp(cmd,"PEER_HELLO")==0) {
        // No-op
    } else if (strcmp(cmd,"HEARTBEAT")==0) {
        // No-op
    }
}

static void handle_line_client(int i, char *line) {
    Client *c = &clients[i];
    char cmd[16];
    if (sscanf(line, "%15s", cmd) != 1) return;

    if (!c->authed) {
        if (strcmp(cmd,"HELLO")==0) {
            // HELLO <user> <room>
            char user[MAX_USER], room[MAX_ROOM];
            if (sscanf(line, "HELLO %63s %63s", user, room) == 2) {
                snprintf(c->user, sizeof c->user, "%s", user);
                snprintf(c->room, sizeof c->room, "%s", room);
                c->authed = true;
                send_fmt(c->fd, "WELCOME %s\n", c->room);
            } else {
                send_fmt(c->fd, "ERR invalid_hello\n");
            }
        } else {
            send_fmt(c->fd, "ERR not_authed\n");
        }
        return;
    }

    if (strcmp(cmd,"PING")==0) {
        send_fmt(c->fd, "PONG\n");
    } else if (strcmp(cmd,"SEND")==0) {
        // SEND <room> <text...>
        // Extract room token then the rest of line as text
        char *p = line + 5; // after "SEND "
        char *room = strtok(p, " \t\r\n");
        char *text = strtok(NULL, "\n");
        if (!room || !text) { send_fmt(c->fd, "ERR invalid_send\n"); return; }

        // Broadcast locally
        char out[MAX_LINE];
        snprintf(out, sizeof out, "EVENT %s %s %s\n", room, c->user, text);
        broadcast_room(room, out);

        // Forward to peer
        relay_to_peer(room, c->user, text);
    } else {
        send_fmt(c->fd, "ERR unknown_cmd\n");
    }
}

static void on_signal(int sig){ (void)sig; running = 0; }

int main(int argc, char **argv) {
    if (argc < 5) die("Usage: %s <listen_port> <peer_host> <peer_port> <node_name>", argv[0]);
    int port = atoi(argv[1]);
    snprintf(PEER_HOST, sizeof PEER_HOST, "%s", argv[2]);
    PEER_PORT = atoi(argv[3]);
    snprintf(NODE, sizeof NODE, "%s", argv[4]);

    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    for (int i=0;i<MAX_CLIENTS;i++) clients[i].fd = -1;

    srv_fd = listen_on(port);
    fprintf(stderr, "[%s] listening on :%d\n", NODE, port);

    // Main loop
    while (running) {
        fd_set rfds; FD_ZERO(&rfds);
        int maxfd = srv_fd;
        FD_SET(srv_fd, &rfds);

        // peer
        if (peer_fd != -1) { FD_SET(peer_fd, &rfds); if (peer_fd > maxfd) maxfd = peer_fd; }

        // clients
        for (int i=0;i<MAX_CLIENTS;i++){
            if (clients[i].fd != -1) {
                FD_SET(clients[i].fd, &rfds);
                if (clients[i].fd > maxfd) maxfd = clients[i].fd;
            }
        }

        struct timeval tv = { .tv_sec = 1, .tv_usec = 0 };
        int n = select(maxfd+1, &rfds, NULL, NULL, &tv);
        if (n < 0) { if (errno==EINTR) continue; perror("select"); break; }

        // periodic peer connect + heartbeat
        time_t now = time(NULL);
        if (peer_fd == -1) try_peer_connect();
        if (peer_fd != -1 && now - last_heartbeat >= 5) {
            char hb[64]; snprintf(hb, sizeof hb, "HEARTBEAT %ld\n", (long)now);
            send(peer_fd, hb, strlen(hb), 0);
            last_heartbeat = now;
        }

        // new clients
        if (FD_ISSET(srv_fd, &rfds)) {
            struct sockaddr_in ca; socklen_t calen = sizeof ca;
            int cfd = accept(srv_fd, (struct sockaddr*)&ca, &calen);
            if (cfd >= 0) {
                int slot=-1;
                for (int i=0;i<MAX_CLIENTS;i++) if (clients[i].fd==-1){slot=i;break;}
                if (slot==-1) { close(cfd); }
                else {
                    clients[slot].fd = cfd; clients[slot].len = 0; clients[slot].authed=false;
                    clients[slot].buf[0]=0; clients[slot].user[0]=0; clients[slot].room[0]=0;
                }
            }
        }

        // peer readable?
        if (peer_fd != -1 && FD_ISSET(peer_fd, &rfds)) {
            char buf[1024]; ssize_t r = recv(peer_fd, buf, sizeof buf - 1, 0);
            if (r <= 0) { close(peer_fd); peer_fd = -1; }
            else {
                buf[r]=0;
                // handle lines
                char *saveptr=NULL;
                char *line = strtok_r(buf, "\n", &saveptr);
                while (line) {
                    handle_line_peer(line);
                    line = strtok_r(NULL, "\n", &saveptr);
                }
            }
        }

        // clients readable?
        for (int i=0;i<MAX_CLIENTS;i++){
            if (clients[i].fd != -1 && FD_ISSET(clients[i].fd, &rfds)) {
                Client *c=&clients[i];
                ssize_t r = recv(c->fd, c->buf + c->len, sizeof(c->buf)-1 - c->len, 0);
                if (r <= 0) { remove_client(i); continue; }
                c->len += (size_t)r; c->buf[c->len]=0;
                // process complete lines
                char *start = c->buf;
                char *nl;
                while ((nl = strchr(start, '\n'))) {
                    *nl = 0;
                    handle_line_client(i, start);
                    start = nl + 1;
                }
                // shift remaining
                size_t remain = c->buf + c->len - start;
                memmove(c->buf, start, remain);
                c->len = remain; c->buf[c->len]=0;
            }
        }
    }

    if (peer_fd!=-1) close(peer_fd);
    if (srv_fd!=-1) close(srv_fd);
    for (int i=0;i<MAX_CLIENTS;i++) if (clients[i].fd!=-1) close(clients[i].fd);
    return 0;
}
