
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

const char* host = "127.0.0.2";
int port = 9009;

const char* send_host = "127.0.0.2";
int send_port = 9003;
int sock_fd, new_fd;

socklen_t addrlen;
struct sockaddr_in my_addr, client_addr;
int status;
int on = 1;

char indata[1024] = {0}, outdata[1024] = {0};
char udp_indata[1024] = {0}, udp_outdata[1024] = {0};

int udp_sock_fd;
struct sockaddr_in serv_name;
int udp_status;

void tcp_recv(){

    
    addrlen = sizeof(client_addr);
    int count = 0;
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
            if (count > 4) break;
        }
        if (count > 4) break;

    }
    // close(sock_fd);

}

void udp_send(){

    udp_outdata[0] = 'a';
    udp_outdata[1] = 'b';
    udp_outdata[2] = 'c';
    udp_outdata[3] = 'd';

    int payload_size = 1500;
    char buf[payload_size];
    for(int i=0 ; i<payload_size ; i++)
    {
    	buf[i] = 1;
    }
    
    for( int ii =0; ii<5 ; ii++) {
        sleep(1);
        udp_outdata[0] ++;
        udp_outdata[1] ++;
        udp_outdata[2] ++;
        udp_outdata[3] ++;
        // Remove newline character if fgets reads it from stdin
        udp_outdata[strcspn(udp_outdata, "\n")] = '\0';

        printf("Send: %s\n", udp_outdata);
        send(udp_sock_fd, udp_outdata, strlen(udp_outdata), 0);

        int nbytes = recv(udp_sock_fd, udp_indata, sizeof(udp_indata) - 1, 0);
        if (nbytes <= 0) {
            close(udp_sock_fd);
            printf("client closed connection.\n");
            break;
        }

        // Ensure buffer is null-terminated to safely use it as a C string
        udp_indata[nbytes] = '\0';
        // printf("Recv: %s\n", indata);
        close(udp_sock_fd);
    }
}


int main()
{
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
    printf("server start at: %s:%d\n", inet_ntoa(my_addr.sin_addr), port);

    status = listen(sock_fd, 5);
    if (status == -1) {
        perror("Listening error");
        exit(1);
    }
    printf("wait for connection...\n");






    sleep(5);



    // create a socket
    udp_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (udp_sock_fd == -1) {
        perror("Socket creation error");
        exit(1);
    }

    // server address
    serv_name.sin_family = AF_INET;
    inet_aton(send_host, &serv_name.sin_addr);
    serv_name.sin_port = htons(send_port);

    udp_status = connect(udp_sock_fd, (struct sockaddr *)&serv_name, sizeof(serv_name));
    if (udp_status == -1) {
        perror("Connection error");
        exit(1);
    }

    sleep(5);

    // pid_t pid = fork();
    int pid = 1;
    if (pid == 0) { // Child process
        tcp_recv();
        return 0;
    } else if (pid > 0) { // Parent process
        udp_send();
        return 0;
    } else {
        perror("Failed to fork");
        return 1;
    }

    return 0;
}