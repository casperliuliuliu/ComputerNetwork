
// g++ cpp-linux-tcp-socket-server.cpp -o server
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <chrono>
#include <ctime> 
#include <thread> 
using namespace std;
const char* host = "127.0.0.1";
int port = 9090;

const char* out_host = "127.0.0.2";
int out_port = 9012;

const char* udp_client_host = "127.0.0.1";
int udp_client_port = 9004;

const char* udp_server_host = "127.0.0.2";
int udp_server_out_port = 9003;

#define MTU 1500
typedef struct TCPHeader {
    chrono::time_point<chrono::system_clock> start;
}TCPHeader; //20bytes

typedef struct Packet  
{
    TCPHeader tcpheader;
    char buffer[MTU - 20];
}Packet;  

typedef struct UDPHeader {
    chrono::time_point<chrono::system_clock> start;
}UDPHeader; //20bytes

typedef struct UDPPacket  
{
  UDPHeader tcpheader;
  char buffer[MTU - 20];
}UDPPacket; 

void tcp_client_to_server(){


    int server_sock_fd;
    struct sockaddr_in serv_name;
    int server_status;
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
    printf("router start at: %s:%d\n", inet_ntoa(my_addr.sin_addr), port);

    status = listen(sock_fd, 5);
    if (status == -1) {
        perror("Listening error");
        exit(1);
    }
    printf("wait for connection...\n");

    addrlen = sizeof(client_addr);
    int count =0;

    struct TCPHeader tcpHeader;
    chrono::time_point<chrono::system_clock> start_time_point;
    chrono::time_point<chrono::system_clock> router_start_time_point;
    chrono::time_point<chrono::system_clock> router_end_time_point;
    time_t start_time;
    char* data;
    chrono::duration<double> elapsed_seconds;
    double history_time;
    double this_time;
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
            router_start_time_point = chrono::system_clock::now();
            memcpy(&tcpHeader, indata, sizeof(TCPHeader));
            data = indata + sizeof(TCPHeader);
            start_time_point = tcpHeader.start;
            start_time = chrono::system_clock::to_time_t(start_time_point);
            // cout << "Timestamp: " << ctime(&start_time);
            // cout << "Received data: " << data << endl;
            count++;

            sprintf(outdata, "%s", indata);
            this_thread::sleep_for(chrono::milliseconds(1500));
            // printf("Send: %s\n", outdata);

            router_end_time_point = chrono::system_clock::now();
            elapsed_seconds = router_end_time_point - router_start_time_point;

            if (count != 1)
                this_time = elapsed_seconds.count() * 0.3 + history_time * 0.7;
            else
                this_time = elapsed_seconds.count();
            history_time = this_time;
            cout << "AVG Packet queuing time: " << this_time << "s" << endl;

            send(server_sock_fd, outdata, strlen(outdata), 0);

            if (count > 4) break;
        }
        if (count > 4) break;

    }
    close(sock_fd);
    close(server_sock_fd);
}

void udp_server_to_client(){
    int server_sock_fd;
    struct sockaddr_in serv_name;
    int server_status;
    // char indata[1024] = {0}, outdata[1024] = {0};

    server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock_fd == -1) {
        perror("Socket creation error");
        exit(1);
    }
    // server address
    serv_name.sin_family = AF_INET;
    inet_aton(udp_client_host, &serv_name.sin_addr);
    serv_name.sin_port = htons(udp_client_port);

    server_status = connect(server_sock_fd, (struct sockaddr *)&serv_name, sizeof(serv_name));
    if (server_status == -1) {
        perror("Connection error");
        exit(1);
    }


    int sock_fd, new_fd;
    socklen_t addrlen;
    struct sockaddr_in my_addr, client_addr;
    int status;
    char indata[2000] = {0}, outdata[2000] = {0};
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
    inet_aton(udp_server_host, &my_addr.sin_addr);
    my_addr.sin_port = htons(udp_server_out_port);

    status = bind(sock_fd, (struct sockaddr *)&my_addr, sizeof(my_addr));
    if (status == -1) {
        perror("Binding error");
        exit(1);
    }
    printf("router start at: %s:%d\n", inet_ntoa(my_addr.sin_addr), udp_server_out_port);

    status = listen(sock_fd, 5);
    if (status == -1) {
        perror("Listening error");
        exit(1);
    }
    printf("wait for connection...\n");

    addrlen = sizeof(client_addr);
    int count = 0;
    struct UDPHeader udpHeader;
    chrono::time_point<chrono::system_clock> start_time_point;
    time_t start_time;
    chrono::time_point<chrono::system_clock> router_start_time_point;
    chrono::time_point<chrono::system_clock> router_end_time_point;
    chrono::duration<double> elapsed_seconds;
    char* data;
    double history_time = 0;
    double this_time = 0;
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
            router_start_time_point = chrono::system_clock::now();
            memcpy(&udpHeader, indata, sizeof(UDPHeader));
            data = indata + sizeof(UDPHeader);
            start_time_point = udpHeader.start;
            start_time = chrono::system_clock::to_time_t(start_time_point);
            // cout << "Received data: " << data << endl;
            count++;

            sprintf(outdata, "%s", indata);
            this_thread::sleep_for(chrono::milliseconds(500));
            // printf("\nSend: %s\n", outdata);
            // cout << "Timestamp: " << ctime(&start_time);
            router_end_time_point = chrono::system_clock::now();
            elapsed_seconds = router_end_time_point - router_start_time_point;
            if (count != 1)
                this_time = elapsed_seconds.count() * 0.3 + history_time * 0.7;
            else
                this_time = elapsed_seconds.count();
            history_time = this_time;
            cout << "AVG Packet queuing time: " << this_time << "s" << endl;
            send(server_sock_fd, outdata, strlen(outdata), 0);

            if (count > 4) break;

        }
        if (count > 4) break;

    }
    close(sock_fd);
    close(server_sock_fd);
}


int main()
{

    tcp_client_to_server();
    sleep(2);
    udp_server_to_client();


    return 0;
}