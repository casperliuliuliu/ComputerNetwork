#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define CLIENT_IP "127.0.0.1"
#define SERVER_IP "127.0.0.2"
#define TCP_FROM_CLIENT 9000
#define TCP_TO_SERVER 9002

// #define ROUTER_PORT_CLIENT_TO_SERVER 9000
// #define CLIENT_PORT 9003
// #define ROUTER_PORT_SERVER_TO_CLIENT 9004
#define BUFFER_SIZE 2048

using namespace std;

void handleTraffic(int recvSockfd, struct sockaddr_in recvAddr, int sendSockfd, struct sockaddr_in sendAddr) {
    char buffer[BUFFER_SIZE];
    socklen_t addrLen = sizeof(recvAddr);
    int n;

    while ((n = recvfrom(recvSockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&recvAddr, &addrLen)) > 0) {
        cout << "Received packet of size " << n << " bytes: ";
        sendto(sendSockfd, buffer, n, 0, (struct sockaddr *)&sendAddr, sizeof(sendAddr));
        cout << "Forwarded a packet of size " << n << " bytes" << endl;
        cout << "buffer" << buffer << endl;
    }

    if (n < 0) {
        perror("Error in receiving data");
    }
}


int main() {
    int tcp_receive;
    struct sockaddr_in addr_tcp_receive;
    struct sockaddr_in client_addr;
    socklen_t addrlen;

    // Create a UDP socket for the router
    tcp_receive = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_receive < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set up the client to server forwarding
    memset(&addr_tcp_receive, 0, sizeof(addr_tcp_receive));
    addr_tcp_receive.sin_family = AF_INET;
    addr_tcp_receive.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_tcp_receive.sin_port = htons(TCP_FROM_CLIENT);
    inet_aton(CLIENT_IP, &addr_tcp_receive.sin_addr);

    if (bind(tcp_receive, (struct sockaddr *)&addr_tcp_receive, sizeof(addr_tcp_receive)) < 0) {
        perror("Bind failed for client to server socket");
        return 1;
    }
    printf("server start at: %s:%d\n", inet_ntoa(addr_tcp_receive.sin_addr), TCP_FROM_CLIENT);
    int status;

    status = listen(tcp_receive, 5);
    if (status == -1) {
        perror("Listening error");
        exit(1);
    }
    char buffer[BUFFER_SIZE];
    int n;

    // while ((n = recv(tcp_receive, buffer, BUFFER_SIZE, 0)) > 0) {
    //     cout << "Received packet of size " << n << " bytes." << endl;
    //     buffer[n] = '\0';  // Ensure the buffer is null-terminated
    //     cout << "Buffer: " << buffer << endl;
    // }
    int new_fd;
    char indata[1024] = {0}, outdata[1024] = {0};

    while (1) {
        new_fd = accept(tcp_receive, (struct sockaddr *)&client_addr, &addrlen);
        printf("connected by %s:%d\n", inet_ntoa(client_addr.sin_addr),
            ntohs(client_addr.sin_port));

        while (1) {
            int nbytes = recv(new_fd, indata, sizeof(indata), 0);
            if (nbytes <= 0) {
                close(new_fd);
                printf("client closed connection.\n");
                break;
            }
            printf("recv: %s\n", indata);

            sprintf(outdata, "echo %s", indata);
            send(new_fd, outdata, strlen(outdata), 0);
        }
    }

    if (n < 0) {
        perror("Recv failed");
    }

    cout << "Ending reception." << endl;
    close(tcp_receive);
    return 0;
}


    // tcp_send = socket(AF_INET, SOCK_DGRAM, 0);
    // if (tcp_send < 0) {
    //     perror("Socket creation failed");
    //     return 1;
    // }

    // // Set up the server to client forwarding
    // memset(&addr_tcp_send, 0, sizeof(addr_tcp_send));
    // addr_tcp_send.sin_family = AF_INET;
    // addr_tcp_send.sin_addr.s_addr = htonl(INADDR_ANY);
    // addr_tcp_send.sin_port = htons(TCP_TO_SERVER);

    // if (bind(tcp_send, (struct sockaddr *)&addr_tcp_send, sizeof(addr_tcp_send)) < 0) {
    //     perror("Bind failed for server to client socket");
    //     return 1;
    // }

    // // Define the server and client addresses to forward the packets
    // memset(&addrServer, 0, sizeof(addrServer));
    // addrServer.sin_family = AF_INET;
    // addrServer.sin_port = htons(SERVER_PORT);
    // inet_pton(AF_INET, SERVER_IP, &addrServer.sin_addr);

    // memset(&addrClient, 0, sizeof(addrClient));
    // addrClient.sin_family = AF_INET;
    // addrClient.sin_port = htons(CLIENT_PORT);
    // inet_pton(AF_INET, CLIENT_IP, &addrClient.sin_addr);

    // Start a child process or thread to handle Client to Server traffic
    // pid_t pid = fork();
    // if (pid == 0) {
    //     // Child process: handle client to server traffic
    //     cout << "handling c to s" << endl;
    //     handleTraffic(sockfdClientToServer, addrClientToServer, sockfdServerToClient, addrServer);
    //     close(sockfdClientToServer);
    //     close(sockfdServerToClient);
    //     return 0;
    // } else {
    //     // Parent process: handle server to client traffic
    //     cout << "handling s to c" << endl;
    //     handleTraffic(sockfdServerToClient, addrServerToClient, sockfdClientToServer, addrClient);
    //     close(sockfdClientToServer);
    //     close(sockfdServerToClient);
    //     return 0;
    // }
