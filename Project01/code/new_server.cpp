#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 9002 // Assuming this is the port the client sends data to
#define BUFFER_SIZE 2048 // Enough to receive the whole packet

// Using structs to help with the parsing of the incoming packets
struct MACHeader {
    uint8_t sour_mac[6];
    uint8_t dest_mac[6];
    uint16_t fram_typ;
    uint32_t crc;
};

struct IPHeader {
    uint8_t version_ihl;
    uint8_t type_of_service;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment_offset;
    uint8_t time_to_live;
    uint8_t protocol;
    uint16_t header_checksum;
    uint32_t source_ip;
    uint32_t destination_ip;
};

struct TCPHeader {
    uint16_t source_port;
    uint16_t destination_port;
    uint32_t sequence_number;
    uint32_t ack_number;
    uint16_t offset_reserved_flags;
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgent_pointer;
};

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Create socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 9002
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT);

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Accept the first connection
    client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (client_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // Read data from the socket
    ssize_t bytes_read = read(client_socket, buffer, BUFFER_SIZE);
    if (bytes_read > 0) {
        std::cout << "Received data from client." << std::endl;

        // Assuming payload starts right after the TCP header in your client's packet structure
        int header_total = sizeof(MACHeader) + sizeof(IPHeader) + sizeof(TCPHeader);
        std::string payload(buffer + header_total, bytes_read - header_total);
        std::cout << "Payload: " << payload << std::endl;
    } else {
        perror("Read failed");
    }

    // Close the socket
    close(client_socket);
    close(server_fd);

    return 0;
}
