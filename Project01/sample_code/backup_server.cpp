
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
int port = 9002;


const char* send_host = "127.0.0.2";
int send_port = 9003;

void tcp_recv(){

    int sock_fd, new_fd;
    socklen_t addrlen;
    struct sockaddr_in my_addr, client_addr;
    int status;
    char indata[1024] = {0}, outdata[1024] = {0};
    int on = 1;

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
    close(sock_fd);

}

void udp_send(){
    int sock_fd;
    struct sockaddr_in serv_name;
    int status;
    char indata[1024] = {0}, outdata[1024] = {0};

    // create a socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("Socket creation error");
        exit(1);
    }

    // server address
    serv_name.sin_family = AF_INET;
    inet_aton(send_host, &serv_name.sin_addr);
    serv_name.sin_port = htons(send_port);

    status = connect(sock_fd, (struct sockaddr *)&serv_name, sizeof(serv_name));
    if (status == -1) {
        perror("Connection error");
        exit(1);
    }
    outdata[0] = 'a';
    outdata[1] = 'b';
    outdata[2] = 'c';
    outdata[3] = 'd';

    int payload_size = 1500;
    char buf[payload_size];
    for(int i=0 ; i<payload_size ; i++)
    {
    	buf[i] = 1;
    }
    
    for( int ii =0; ii<5 ; ii++) {
        sleep(1);
        outdata[0] ++;
        outdata[1] ++;
        outdata[2] ++;
        outdata[3] ++;
        // Remove newline character if fgets reads it from stdin
        outdata[strcspn(outdata, "\n")] = '\0';

        printf("Send: %s\n", outdata);
        send(sock_fd, outdata, strlen(outdata), 0);

        int nbytes = recv(sock_fd, indata, sizeof(indata) - 1, 0);
        if (nbytes <= 0) {
            close(sock_fd);
            printf("client closed connection.\n");
            break;
        }

        // Ensure buffer is null-terminated to safely use it as a C string
        indata[nbytes] = '\0';
        // printf("Recv: %s\n", indata);
    }
}
int main()
{
    tcp_recv();
    sleep(3);
    udp_send();
    return 0;
}