// g++ cpp-linux-tcp-socket-client.cpp -o client
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

const char* host = "127.0.0.1";
int port = 9090;

const char* recv_host = "127.0.0.1";
int recv_port = 9004;

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

int sock_fd;
struct sockaddr_in serv_name;
int status;
char indata[2000] = {0}, outdata[1024] = {0};

int udp_sock_fd, new_fd;
socklen_t addrlen;
struct sockaddr_in my_addr, client_addr;
int udp_status;
int on = 1;


void tcp_send(){
    Packet mypacket;
    memset( mypacket.buffer, 0, sizeof(mypacket.buffer));
	char buf[BUFF_LEN];
    
    // cout << mypacket.buffer << endl;
    outdata[0] = 'a';
    outdata[1] = 'b';
    outdata[2] = 'c';
    outdata[3] = 'd';

    for( int ii =0; ii<5 ; ii++) {
        sleep(3);
        TCPHeader tcpHeader;
        tcpHeader.start = chrono::system_clock::now();

        memcpy(buf, &tcpHeader, sizeof(TCPHeader));
        memcpy(buf + sizeof(TCPHeader), outdata, sizeof(outdata));

        cout << "send: " << outdata << endl;
        cout << "===================" << endl;
        send(sock_fd, buf, strlen(buf), 0);

        outdata[0] ++;
        outdata[1] ++;
        outdata[2] ++;
        outdata[3] ++;

    }
}
void udp_recv(){

    addrlen = sizeof(client_addr);
    int count = 0;
    UDPHeader udpHeader;
    char* data;
    chrono::time_point<chrono::system_clock> start_time_point;
    time_t start_time;
    chrono::time_point<chrono::system_clock> current_time_point;
    time_t current_time;
    chrono::duration<double> elapsed_seconds;
    while (1) {
        new_fd = accept(udp_sock_fd, (struct sockaddr *)&client_addr, &addrlen);
        printf("connected by %s:%d\n", inet_ntoa(client_addr.sin_addr),
            ntohs(client_addr.sin_port));

        while (1) {
            int nbytes = recv(new_fd, indata, sizeof(indata), 0);
            if (nbytes <= 0) {
                close(new_fd);
                printf("client closed connection.\n");
                break;
            }
            memcpy(&udpHeader, indata, sizeof(UDPHeader));
            data = indata + sizeof(UDPHeader);
            start_time_point = udpHeader.start;
            start_time = chrono::system_clock::to_time_t(start_time_point);
            current_time_point = chrono::system_clock::now();
            current_time = chrono::system_clock::to_time_t(current_time_point);
            elapsed_seconds = current_time_point - start_time_point;

            // cout << "Received data: " << data << endl;
            cout << "Timestamp: " << ctime(&start_time);
            cout << "# " << count+1 << " Packet delay: " << elapsed_seconds.count()*1000 << " ms" << endl;
            cout << "Throughput: " << 1500 * 8 / elapsed_seconds.count() / 1000000 << "Mbps" << endl;
            cout << "===================" << endl;
            count++;


            if (count > 4) break;
        }
        if (count > 4) break;

    }
    close(udp_sock_fd);

}


int main()
{

    // tcp_send();
    // udp_recv();
    cout << "===================" << endl;
    pid_t pid = fork();
    if (pid == 0) { // Child process
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
        tcp_send();
    } 

    else if (pid > 0) { // Parent process
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
        udp_recv();

    } 
    else {
        perror("Failed to fork");
        return 1;
    }

    return 0;
}