#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 9090  // Changed port to 9090
#define MAX_CLIENTS 10
#define BUF_SIZE 512

int main() {
    WSADATA wsaData;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    int client_addr_len = sizeof(client_address);
    char buffer[BUF_SIZE];
    int opt = 1;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error %d\n", WSAGetLastError());
        return 1;
    }

    // Create the server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Set socket option to reuse address in case of address conflict
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0) {
        printf("setsockopt failed with error %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Prepare the server address structure
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Bind the socket to the address and port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        printf("Bind failed with error %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Start listening for incoming connections
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        printf("Listen failed with error %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept client connections
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_addr_len);
    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed with error %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Client connected from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    // Communicate with the client
    while (1) {
        memset(buffer, 0, BUF_SIZE);  // Clear the buffer

        // Receive data from client
        int recv_len = recv(client_socket, buffer, BUF_SIZE, 0);
        if (recv_len == SOCKET_ERROR) {
            printf("Receive failed with error %d\n", WSAGetLastError());
            break;
        }

        if (recv_len == 0) {
            printf("Client disconnected.\n");
            break;
        }

        printf("Received from client: %s\n", buffer);

        // Send the received message back to the client
        if (send(client_socket, buffer, recv_len, 0) == SOCKET_ERROR) {
            printf("Send failed with error %d\n", WSAGetLastError());
            break;
        }
    }

    // Close the sockets and clean up
    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
