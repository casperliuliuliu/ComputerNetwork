#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.2"
#define SERVER_PORT 9000
#define ROUTER_PORT_CLIENT_TO_SERVER 9002
#define BUFFER_SIZE 2048

int forwardTCP(int clientSockfd, const char* serverIp, int serverPort) {
    int serverSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSockfd < 0) {
        perror("Socket creation failed for server connection");
        return -1;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIp, &serverAddr.sin_addr);

    if (connect(serverSockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection to server failed");
        close(serverSockfd);
        return -1;
    }

    char buffer[BUFFER_SIZE];
    int n;

    // Forwarding loop
    while ((n = read(clientSockfd, buffer, BUFFER_SIZE)) > 0) {
        if (write(serverSockfd, buffer, n) != n) {
            perror("Failed to send data to server");
            break;
        }

        // Optionally, read the response back from the server to the client
        int m = read(serverSockfd, buffer, BUFFER_SIZE);
        if (m > 0) {
            if (write(clientSockfd, buffer, m) != m) {
                perror("Failed to send data back to client");
                break;
            }
        }
    }

    if (n < 0) {
        perror("Read from client failed");
    }

    close(serverSockfd);
    return 0;
}

int main() {
    int listenSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in addrClientToServer;
    memset(&addrClientToServer, 0, sizeof(addrClientToServer));
    addrClientToServer.sin_family = AF_INET;
    addrClientToServer.sin_addr.s_addr = htonl(INADDR_ANY);
    addrClientToServer.sin_port = htons(ROUTER_PORT_CLIENT_TO_SERVER);

    if (bind(listenSockfd, (struct sockaddr *)&addrClientToServer, sizeof(addrClientToServer)) < 0) {
        perror("Bind failed for client to server socket");
        return 1;
    }

    if (listen(listenSockfd, 5) < 0) {
        perror("Listen failed");
        close(listenSockfd);
        return 1;
    }

    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSockfd = accept(listenSockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSockfd < 0) {
            perror("Accept failed");
            continue;
        }

        // Handle client connection in a separate process or thread for scalability
        pid_t pid = fork();
        if (pid == 0) { // child process
            close(listenSockfd);
            forwardTCP(clientSockfd, SERVER_IP, SERVER_PORT);
            close(clientSockfd);
            return 0;
        } else if (pid > 0) { // parent process
            close(clientSockfd);
        } else {
            perror("Failed to fork");
        }
    }

    close(listenSockfd);
    return 0;
}
