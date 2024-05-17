
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
using namespace std;

const char* host = "127.0.0.2";
int port = 9014;


const char* send_host = "127.0.0.2";
int send_port = 9003;

#define BUFF_LEN 10000
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


int sock_fd, new_fd;

socklen_t addrlen;
struct sockaddr_in my_addr, client_addr;
int status;
int on = 1;

char indata[1024] = {0}, outdata[1024] = {0};

int udp_sock_fd;
struct sockaddr_in serv_name;
int udp_status;

void tcp_recv(){

    addrlen = sizeof(client_addr);
    int count = 0;
    TCPHeader tcpHeader;
    char* data;
    chrono::time_point<chrono::system_clock> start_time_point;
    time_t start_time;
    chrono::time_point<chrono::system_clock> current_time_point;
    time_t current_time;
    chrono::duration<double> elapsed_seconds;

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
            memcpy(&tcpHeader, indata, sizeof(TCPHeader));
            data = indata + sizeof(TCPHeader);
            start_time_point = tcpHeader.start;
            start_time = chrono::system_clock::to_time_t(start_time_point);
            current_time_point = chrono::system_clock::now();
            current_time = chrono::system_clock::to_time_t(current_time_point);
            elapsed_seconds = current_time_point - start_time_point;

            // cout << "Received data: " << data << endl;
            cout << "Timestamp: " << ctime(&start_time);
            cout << "# " << count+1 << " Packet delay: " << elapsed_seconds.count()*1000 << " ms" << endl;
            cout << "Throughput: " << (sizeof(data) + 20) * 8 / elapsed_seconds.count() / 1000000 << "Mbps" << endl;
            cout << "===================" << endl;

            count++;

            if (count > 4) break;
        }
        if (count > 4) break;

    }
    close(sock_fd);

}

void udp_send(){
    UDPPacket mypacket;
    memset( mypacket.buffer, 0, sizeof(mypacket.buffer));
	char buf[BUFF_LEN];

    int payload_size = 500;
    char udp_outdata[payload_size];
    
    
    for( int ii =0; ii<5 ; ii++) {
        for(int jj=0 ; jj<payload_size ; jj++)
        {
            udp_outdata[jj] = '0' + ii;
        }
        sleep(3);
        UDPHeader udpHeader;
        udpHeader.start = chrono::system_clock::now();

        memcpy(buf, &udpHeader, sizeof(UDPHeader));
        memcpy(buf + sizeof(TCPHeader), udp_outdata, sizeof(udp_outdata));

        cout << "send: " << udp_outdata << endl;
        cout << "===================" << endl;
        send(udp_sock_fd, buf, strlen(buf), 0);

    }
}
int main()
{
    // tcp_recv();
    // sleep(3);
    // udp_send();
    cout << "===================" << endl;

    pid_t pid = fork();
    if (pid == 0) { // Child process
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
        
        tcp_recv();
    } 

    else if (pid > 0) { // Parent process
        sleep(10);

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
        udp_send();
    } 
    else {
        perror("Failed to fork");
        return 1;
    }

    return 0;
}