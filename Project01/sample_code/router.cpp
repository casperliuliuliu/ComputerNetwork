
// g++ cpp-linux-tcp-socket-server.cpp -o server
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;
const char* host = "127.0.0.1";
int port = 9000;

const char* out_host = "127.0.0.2";
int out_port = 9009;

const char* udp_client_host = "127.0.0.1";
int udp_client_port = 9004;

const char* udp_server_host = "127.0.0.2";
int udp_server_out_port = 9003;

int server_sock_fd;
struct sockaddr_in serv_name;
int server_status;
int sock_fd, new_fd;
socklen_t addrlen;
struct sockaddr_in my_addr, client_addr;
int status;
char indata[1024] = {0}, outdata[1024] = {0};
int on = 1;



int udp_server_sock_fd;
struct sockaddr_in udp_serv_name;
int udp_server_status;
int udp_sock_fd, udp_new_fd;
socklen_t udp_addrlen;
struct sockaddr_in udp_my_addr, udp_client_addr;
int udp_status;
char udp_indata[1024] = {0}, udp_outdata[1024] = {0};
int udp_on = 1;

void tcp_client_to_server(){
    
    addrlen = sizeof(client_addr);
    int count =0;
    while (1) {
        new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &addrlen);
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
            count++;
            sprintf(outdata, "%s", indata);

            
            send(new_fd, outdata, strlen(outdata), 0);

            printf("Send: %s\n", outdata);
            send(server_sock_fd, outdata, strlen(outdata), 0);
            if (count > 4) break;
        }
        if (count > 4) break;

    }
    close(sock_fd);
    close(server_sock_fd);
}

void udp_server_to_client(){
    
    udp_addrlen = sizeof(udp_client_addr);
    int count =0;
    while (1) {
        udp_new_fd = accept(udp_sock_fd, (struct sockaddr *)&udp_client_addr, &udp_addrlen);
        printf("connected by %s:%d\n", inet_ntoa(udp_client_addr.sin_addr),
            ntohs(udp_client_addr.sin_port));

        while (1) {
            int nbytes = recv(udp_new_fd, udp_indata, sizeof(udp_indata), 0);
            if (nbytes <= 0) {
                close(udp_new_fd);
                printf("client closed connection.\n");
                break;
            }
            count++;
            printf("recv: %s\n", udp_indata);
            sprintf(udp_outdata, "%s", udp_indata);

            
            send(udp_new_fd, udp_outdata, strlen(udp_outdata), 0);
            printf("Send: %s\n", udp_outdata);
            send(udp_server_sock_fd, udp_outdata, strlen(udp_outdata), 0);
            if (count > 4) break;

        }
        if (count > 4) break;

    }
    close(udp_sock_fd);
    close(udp_server_sock_fd);
}


int main()
{


    // char indata[1024] = {0}, outdata[1024] = {0};

    server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock_fd == -1) {
        perror("Socket creation error");
        exit(1);
    }

    // server address
    serv_name.sin_family = AF_INET;
    inet_aton(out_host, &serv_name.sin_addr);
    serv_name.sin_port = htons(out_port);

    server_status = connect(server_sock_fd, (struct sockaddr *)&serv_name, sizeof(serv_name));
    if (server_status == -1) {
        perror("Connection error");
        exit(1);
    }

    // create a socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("Socket creation error");
        exit(1);
    }

    // for "Address already in use" error message
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) == -1) {
        perror("Setsockopt error");
        exit(1);
    }

    // server address
    my_addr.sin_family = AF_INET;
    inet_aton(host, &my_addr.sin_addr);
    my_addr.sin_port = htons(port);

    status = bind(sock_fd, (struct sockaddr *)&my_addr, sizeof(my_addr));
    if (status == -1) {
        perror("Binding error");
        exit(1);
    }
    printf("router start at: %s:%d\n", inet_ntoa(my_addr.sin_addr), port);

    status = listen(sock_fd, 5);
    if (status == -1) {
        perror("Listening error");
        exit(1);
    }
    printf("wait for connection...\n");



    sleep(5);

    // char indata[1024] = {0}, outdata[1024] = {0};

    udp_server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (udp_server_sock_fd == -1) {
        perror("Socket creation error");
        exit(1);
    }
    // server address
    udp_serv_name.sin_family = AF_INET;
    inet_aton(udp_client_host, &udp_serv_name.sin_addr);
    udp_serv_name.sin_port = htons(udp_client_port);

    udp_server_status = connect(udp_server_sock_fd, (struct sockaddr *)&udp_serv_name, sizeof(udp_serv_name));
    if (udp_server_status == -1) {
        perror("Connection error");
        exit(1);
    }


    // create a socket
    udp_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (udp_sock_fd == -1) {
        perror("Socket creation error");
        exit(1);
    }

    // for "Address already in use" error message
    if (setsockopt(udp_sock_fd, SOL_SOCKET, SO_REUSEADDR, &udp_on, sizeof(int)) == -1) {
        perror("Setsockopt error");
        exit(1);
    }

    // server address
    udp_my_addr.sin_family = AF_INET;
    inet_aton(udp_server_host, &my_addr.sin_addr);
    udp_my_addr.sin_port = htons(udp_server_out_port);

    udp_status = bind(udp_sock_fd, (struct sockaddr *)&udp_my_addr, sizeof(udp_my_addr));
    if (udp_status == -1) {
        perror("Binding error");
        exit(1);
    }
    printf("router start at: %s:%d\n", inet_ntoa(udp_my_addr.sin_addr), udp_server_out_port);

    udp_status = listen(udp_sock_fd, 5);
    if (udp_status == -1) {
        perror("Listening error");
        exit(1);
    }
    printf("wait for connection...\n");


    pid_t pid = fork();
    if (pid == 0) { // Child process
        cout << "handling c to s" << endl;
        tcp_client_to_server();
        return 0;
    } else if (pid > 0) { // Parent process
        cout << "handling s to c" << endl;
        udp_server_to_client();
        return 0;
    } else {
        perror("Failed to fork");
        return 1;
    }


    return 0;
}