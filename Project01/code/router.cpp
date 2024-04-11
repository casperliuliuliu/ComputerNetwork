// #include <iostream>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <cstring>
// #include <unistd.h>
// #include <arpa/inet.h>

// #define CLIENT_IP "127.0.0.1"
// #define SERVER_IP "127.0.0.2"
// #define SERVER_PORT 9000
// #define CLIENT_PORT 9003
// #define BUFFER_SIZE 2048

// using namespace std;

// void handleTraffic(int srcSockfd, int destSockfd) {
//     char buffer[BUFFER_SIZE];
//     ssize_t n;

//     // Read from source and write to destination
//     while ((n = recv(srcSockfd, buffer, BUFFER_SIZE, 0)) > 0) {
//         send(destSockfd, buffer, n, 0);
//     }

//     if (n < 0) {
//         perror("Error in receiving data");
//     }

//     close(srcSockfd);
//     close(destSockfd);
// }

// int main() {
//     int listenSockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (listenSockfd < 0) {
//         perror("Socket creation failed");
//         return 1;
//     }

//     struct sockaddr_in listenAddr;
//     memset(&listenAddr, 0, sizeof(listenAddr));
//     listenAddr.sin_family = AF_INET;
//     listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//     listenAddr.sin_port = htons(CLIENT_PORT); // Listen on the client port

//     if (bind(listenSockfd, (struct sockaddr *)&listenAddr, sizeof(listenAddr)) < 0) {
//         perror("Bind failed");
//         close(listenSockfd);
//         return 1;
//     }

//     if (listen(listenSockfd, 5) < 0) {
//         perror("Listen failed");
//         close(listenSockfd);
//         return 1;
//     }

//     while (true) {
//         int clientSockfd = accept(listenSockfd, NULL, NULL);
//         if (clientSockfd < 0) {
//             perror("Failed to accept connection");
//             continue;
//         }

//         // Connect to the server for each client connection
//         int serverSockfd = socket(AF_INET, SOCK_STREAM, 0);
//         if (serverSockfd < 0) {
//             perror("Server socket creation failed");
//             close(clientSockfd);
//             continue;
//         }

//         struct sockaddr_in serverAddr;
//         memset(&serverAddr, 0, sizeof(serverAddr));
//         serverAddr.sin_family = AF_INET;
//         serverAddr.sin_port = htons(SERVER_PORT);
//         inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

//         if (connect(serverSockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
//             perror("Connect to server failed");
//             close(clientSockfd);
//             close(serverSockfd);
//             continue;
//         }

//         // Handle traffic between client and server
//         handleTraffic(clientSockfd, serverSockfd);
//     }

//     close(listenSockfd);
//     return 0;
// }



// #include <iostream>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <cstring>
// #include <unistd.h>
// #include <arpa/inet.h>

// #define CLIENT_IP "127.0.0.1"
// #define SERVER_IP "127.0.0.2"
// #define SERVER_PORT 9000
// #define ROUTER_PORT 9002  // Use the same port for both directions
// #define CLIENT_PORT 9003
// #define BUFFER_SIZE 2048

// using namespace std;

// void handleTraffic(int sockfd, struct sockaddr_in srcAddr, struct sockaddr_in dstAddr) {
//     char buffer[BUFFER_SIZE];
//     socklen_t addrLen = sizeof(srcAddr);
//     int n;

//     // Handle traffic in a loop
//     while ((n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&srcAddr, &addrLen)) > 0) {
//         cout << "Received packet of size " << n << " bytes: ";
//         sendto(sockfd, buffer, n, 0, (struct sockaddr *)&dstAddr, sizeof(dstAddr));
//         cout << "Forwarded a packet of size " << n << " bytes" << endl;
//         cout.write(buffer, n);
//         cout << endl;

//     }

//     if (n < 0) {
//         perror("Error in receiving data");
//     }
// }

// int main() {
//     int sockfd;
//     struct sockaddr_in routerAddr, serverAddr, clientAddr;

//     // Create a single socket for the router
//     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//     if (sockfd < 0) {
//         perror("Socket creation failed");
//         return 1;
//     }

//     // Set up the router's socket
//     memset(&routerAddr, 0, sizeof(routerAddr));
//     routerAddr.sin_family = AF_INET;
//     routerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//     routerAddr.sin_port = htons(ROUTER_PORT);

//     if (bind(sockfd, (struct sockaddr *)&routerAddr, sizeof(routerAddr)) < 0) {
//         perror("Bind failed for router socket");
//         return 1;
//     }

//     // Define the server and client addresses
//     memset(&serverAddr, 0, sizeof(serverAddr));
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_port = htons(SERVER_PORT);
//     inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
//     cout << "finish copy server" << endl;

//     memset(&clientAddr, 0, sizeof(clientAddr));
//     clientAddr.sin_family = AF_INET;
//     clientAddr.sin_port = htons(CLIENT_PORT);
//     inet_pton(AF_INET, CLIENT_IP, &clientAddr.sin_addr);
//     cout << "finish copy client" << endl;

//     // Start a child process to handle traffic from client to server
//     pid_t pid = fork();
//     if (pid == 0) { // Child process
//         cout << "handling c to s" << endl;
//         handleTraffic(sockfd, clientAddr, serverAddr);
//         close(sockfd);
//         return 0;
//     } else if (pid > 0) { // Parent process
//         cout << "handling s to c" << endl;
//         handleTraffic(sockfd, serverAddr, clientAddr);
//         close(sockfd);
//         return 0;
//     } else {
//         perror("Failed to fork");
//         close(sockfd);
//         return 1;
//     }
// }


// #include <iostream>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <cstring>
// #include <unistd.h>
// #include <arpa/inet.h>

// #define CLIENT_IP "127.0.0.1"
// #define SERVER_IP "127.0.0.2"
// #define SERVER_PORT 9000
// #define ROUTER_PORT_CLIENT_TO_SERVER 9002
// #define CLIENT_PORT 9003
// #define ROUTER_PORT_SERVER_TO_CLIENT 9004
// #define BUFFER_SIZE 2048

// using namespace std;

// void relayTraffic(int clientSockfd, int serverSockfd) {
//     char buffer[BUFFER_SIZE];
//     ssize_t n;

//     // Relay from client to server
//     while ((n = recv(clientSockfd, buffer, BUFFER_SIZE, 0)) > 0) {
//         if (send(serverSockfd, buffer, n, 0) < 0) {
//             perror("Failed to send to server");
//             break;
//         }
//     }

//     if (n < 0) {
//         perror("Error in receiving data from client");
//     }

//     close(clientSockfd);
//     close(serverSockfd);
// }

// void handleTraffic(int recvSockfd, struct sockaddr_in recvAddr, int sendSockfd, struct sockaddr_in sendAddr) {
//     char buffer[BUFFER_SIZE];
//     socklen_t addrLen = sizeof(recvAddr);
//     int n;

//     while ((n = recvfrom(recvSockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&recvAddr, &addrLen)) > 0) {
//         cout << "Received packet of size " << n << " bytes: ";
//         sendto(sendSockfd, buffer, n, 0, (struct sockaddr *)&sendAddr, sizeof(sendAddr));
//         cout << "Forwarded a packet of size " << n << " bytes" << endl;
//     }

//     if (n < 0) {
//         perror("Error in receiving data");
//     }
// }

// int main() {
//     int listenSockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (listenSockfd < 0) {
//         perror("Socket creation failed");
//         return 1;
//     }

//     struct sockaddr_in listenAddr;
//     memset(&listenAddr, 0, sizeof(listenAddr));
//     listenAddr.sin_family = AF_INET;
//     listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//     listenAddr.sin_port = htons(ROUTER_PORT_CLIENT_TO_SERVER);

//     if (bind(listenSockfd, (struct sockaddr *)&listenAddr, sizeof(listenAddr)) < 0) {
//         perror("Bind failed");
//         close(listenSockfd);
//         return 1;
//     }

//     if (listen(listenSockfd, 5) < 0) {
//         perror("Listen failed");
//         close(listenSockfd);
//         return 1;
//     }

//     while (true) {
//         int clientSockfd = accept(listenSockfd, NULL, NULL);
//         if (clientSockfd < 0) {
//             perror("Failed to accept connection");
//             continue;
//         }

//         // Connect to the server
//         int serverSockfd = socket(AF_INET, SOCK_STREAM, 0);
//         struct sockaddr_in serverAddr;
//         memset(&serverAddr, 0, sizeof(serverAddr));
//         serverAddr.sin_family = AF_INET;
//         serverAddr.sin_port = htons(SERVER_PORT);
//         inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

//         if (connect(serverSockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
//             perror("Connect to server failed");
//             close(clientSockfd);
//             close(serverSockfd);
//             continue;
//         }

//         // Relay traffic between the client and server
//         relayTraffic(clientSockfd, serverSockfd);
//     }

//     close(listenSockfd);
//     return 0;
//     // int sockfdClientToServer, sockfdServerToClient;
//     // struct sockaddr_in addrClientToServer, addrServerToClient, addrServer, addrClient;

//     // // Create sockets for the router
//     // sockfdClientToServer = socket(AF_INET, SOCK_STREAM, 0);
//     // sockfdServerToClient = socket(AF_INET, SOCK_STREAM, 0);

//     // if (sockfdClientToServer < 0 || sockfdServerToClient < 0) {
//     //     perror("Socket creation failed");
//     //     return 1;
//     // }

//     // // Set up the client to server forwarding
//     // memset(&addrClientToServer, 0, sizeof(addrClientToServer));
//     // addrClientToServer.sin_family = AF_INET;
//     // addrClientToServer.sin_addr.s_addr = htonl(INADDR_ANY);
//     // addrClientToServer.sin_port = htons(ROUTER_PORT_CLIENT_TO_SERVER);

//     // if (bind(sockfdClientToServer, (struct sockaddr *)&addrClientToServer, sizeof(addrClientToServer)) < 0) {
//     //     perror("Bind failed for client to server socket");
//     //     return 1;
//     // }

//     // // Set up the server to client forwarding
//     // memset(&addrServerToClient, 0, sizeof(addrServerToClient));
//     // addrServerToClient.sin_family = AF_INET;
//     // addrServerToClient.sin_addr.s_addr = htonl(INADDR_ANY);
//     // addrServerToClient.sin_port = htons(ROUTER_PORT_SERVER_TO_CLIENT);

//     // if (bind(sockfdServerToClient, (struct sockaddr *)&addrServerToClient, sizeof(addrServerToClient)) < 0) {
//     //     perror("Bind failed for server to client socket");
//     //     return 1;
//     // }

//     // // Define the server and client addresses to forward the packets
//     // memset(&addrServer, 0, sizeof(addrServer));
//     // addrServer.sin_family = AF_INET;
//     // addrServer.sin_port = htons(SERVER_PORT);
//     // inet_pton(AF_INET, SERVER_IP, &addrServer.sin_addr);

//     // memset(&addrClient, 0, sizeof(addrClient));
//     // addrClient.sin_family = AF_INET;
//     // addrClient.sin_port = htons(CLIENT_PORT);
//     // inet_pton(AF_INET, CLIENT_IP, &addrClient.sin_addr);

//     // // Start a child process or thread to handle Client to Server traffic
//     // pid_t pid = fork();
//     // if (pid == 0) {
//     //     // Child process: handle client to server traffic
//     //     cout << "handling c to s" << endl;
//     //     handleTraffic(sockfdClientToServer, addrClientToServer, sockfdServerToClient, addrServer);
//     //     close(sockfdClientToServer);
//     //     close(sockfdServerToClient);
//     //     return 0;
//     // } else {
//     //     // Parent process: handle server to client traffic
//     //     cout << "handling s to c" << endl;
//     //     handleTraffic(sockfdServerToClient, addrServerToClient, sockfdClientToServer, addrClient);
//     //     close(sockfdClientToServer);
//     //     close(sockfdServerToClient);
//     //     return 0;
//     // }
// }





#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.2"
#define SERVER_PORT 9000
#define ROUTER_LISTEN_PORT 9002
#define BUFFER_SIZE 2048

using namespace std;

// Function to relay traffic between two sockets
void relayTraffic(int clientSockfd, int serverSockfd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    while (true) {
        // Receive from client
        bytesRead = recv(clientSockfd, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0) break; // Break if connection closed or error

        // Send to server
        if (send(serverSockfd, buffer, bytesRead, 0) == -1) {
            perror("Error sending data to server");
            break;
        }

        // Receive from server
        bytesRead = recv(serverSockfd, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0) break; // Break if connection closed or error

        // Send back to client
        if (send(clientSockfd, buffer, bytesRead, 0) == -1) {
            perror("Error sending data to client");
            break;
        }
    }

    close(clientSockfd);
    close(serverSockfd);
}

int main() {
    int listenSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in routerAddr;
    memset(&routerAddr, 0, sizeof(routerAddr));
    routerAddr.sin_family = AF_INET;
    routerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    routerAddr.sin_port = htons(ROUTER_LISTEN_PORT);

    if (bind(listenSockfd, (struct sockaddr *)&routerAddr, sizeof(routerAddr)) < 0) {
        perror("Bind failed");
        close(listenSockfd);
        return 1;
    }

    if (listen(listenSockfd, 5) < 0) {
        perror("Listen failed");
        close(listenSockfd);
        return 1;
    }

    cout << "Router is listening for connections on port " << ROUTER_LISTEN_PORT << endl;

    while (true) {
        int clientSockfd = accept(listenSockfd, NULL, NULL);
        if (clientSockfd < 0) {
            perror("Failed to accept connection");
            continue;
        }

        int serverSockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSockfd < 0) {
            perror("Server socket creation failed");
            close(clientSockfd);
            continue;
        }

        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(SERVER_PORT);
        inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

        if (connect(serverSockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("Connection to server failed");
            close(clientSockfd);
            close(serverSockfd);
            continue;
        }

        // Start relaying traffic between the client and server
        relayTraffic(clientSockfd, serverSockfd);
    }

    close(listenSockfd);
    return 0;
}
