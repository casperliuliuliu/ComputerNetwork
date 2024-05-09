// g++ cpp-linux-tcp-socket-client.cpp -o client
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

const char* recv_host = "127.0.0.1";
int recv_port = 9004;

int sock_fd;
struct sockaddr_in serv_name;
int status;
char indata[1024] = {0}, outdata[1024] = {0};

int udp_sock_fd, new_fd;
socklen_t addrlen;
struct sockaddr_in my_addr, client_addr;
int udp_status;
char udp_indata[1024] = {0}, udp_outdata[1024] = {0};
int on = 1;

void tcp_send(){

    outdata[0] = 'a';
    outdata[1] = 'b';
    outdata[2] = 'c';
    outdata[3] = 'd';
    for( int ii =0; ii<5 ; ii++) {
        sleep(1);

        
        // Remove newline character if fgets reads it from stdin
        outdata[strcspn(outdata, "\n")] = '\0';

        printf("Send: %s\n", outdata);
        send(sock_fd, outdata, strlen(outdata), 0);

        int nbytes = recv(sock_fd, indata, sizeof(indata) - 1, 0);
        if (nbytes <= 0) {
            close(sock_fd);
            printf("Server closed connection.\n");
            break;
        }
        outdata[0] ++;
        outdata[1] ++;
        outdata[2] ++;
        outdata[3] ++;
        // Ensure buffer is null-terminated to safely use it as a C string
        indata[nbytes] = '\0';
        // printf("Recv: %s\n", indata);
        close(sock_fd);
    }
}
void udp_recv(){


    addrlen = sizeof(client_addr);
    int count = 0;
    while (1) {
        new_fd = accept(udp_sock_fd, (struct sockaddr *)&client_addr, &addrlen);
        printf("connected by %s:%d\n", inet_ntoa(client_addr.sin_addr),
            ntohs(client_addr.sin_port));

        while (1) {
            int nbytes = recv(new_fd, udp_indata, sizeof(udp_indata), 0);
            if (nbytes <= 0) {
                close(new_fd);
                printf("client closed connection.\n");
                break;
            }
            printf("recv: %s\n", udp_indata);
            count++;
            if (count > 4) break;
        }
        if (count > 4) break;

    }
    // close(udp_sock_fd);

}
int main()
{
    // create a socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("Socket creation error");
        exit(1);
    }

    // server address
    serv_name.sin_family = AF_INET;
    inet_aton(host, &serv_name.sin_addr);
    serv_name.sin_port = htons(port);

    status = connect(sock_fd, (struct sockaddr *)&serv_name, sizeof(serv_name));
    if (status == -1) {
        perror("Connection error");
        exit(1);
    }


    //UDP

    // create a socket
    udp_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (udp_sock_fd == -1) {
        perror("Socket creation error");
        exit(1);
    }
    // for "Address already in use" error message
    if (setsockopt(udp_sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) == -1) {
        perror("Setsockopt error");
        exit(1);
    }
    // server address
    my_addr.sin_family = AF_INET;
    inet_aton(recv_host, &my_addr.sin_addr);
    my_addr.sin_port = htons(recv_port);

    udp_status = bind(udp_sock_fd, (struct sockaddr *)&my_addr, sizeof(my_addr));
    if (udp_status == -1) {
        perror("Binding error");
        exit(1);
    }
    printf("client start at: %s:%d\n", inet_ntoa(my_addr.sin_addr), recv_port);

    udp_status = listen(udp_sock_fd, 5);
    if (udp_status == -1) {
        perror("Listening error");
        exit(1);
    }
    printf("wait for connection...\n");

    sleep(10);



    // pid_t pid = fork();
    int pid = 1;
    if (pid == 0) { // Child process
        // cout << "handling c to s" << endl;
        tcp_send();
        return 0;
    } else if (pid > 0) { // Parent process
        // cout << "" << endl;
        udp_recv();
        return 0;
    } else {
        perror("Failed to fork");
        return 1;
    }

    return 0;
}