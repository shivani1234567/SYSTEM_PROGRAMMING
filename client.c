#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib for Winsock functions

#define PORT 9090
#define BUF_SIZE 512

int main() {
    WSADATA wsaData;
    SOCKET client_socket;
    struct sockaddr_in server_address;
    char buffer[BUF_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error %d\n", WSAGetLastError());
        return 1;
    }

    // Create the client socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("Socket creation failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Prepare the server address structure
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Use inet_addr() to convert IP address to binary format (for IPv4)
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (server_address.sin_addr.s_addr == INADDR_NONE) {
        printf("Invalid address/ Address not supported\n");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        printf("Connection failed with error %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("Connected to the server. Type your message:\n");

    // Main loop to send and receive data
    while (1) {
        // Clear the buffer and read user input
        memset(buffer, 0, BUF_SIZE);
        printf("Enter message: ");
        fgets(buffer, BUF_SIZE, stdin);

        // Remove the newline character from the input
        buffer[strcspn(buffer, "\n")] = '\0';

        // Send the data to the server
        if (send(client_socket, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
            printf("Send failed with error %d\n", WSAGetLastError());
            break;
        }

        // Receive the server's response
        memset(buffer, 0, BUF_SIZE);
        int recv_len = recv(client_socket, buffer, BUF_SIZE, 0);
        if (recv_len == SOCKET_ERROR) {
            printf("Receive failed with error %d\n", WSAGetLastError());
            break;
        }

        if (recv_len == 0) {
            printf("Server disconnected.\n");
            break;
        }

        printf("Server: %s\n", buffer);
    }

    // Clean up
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
