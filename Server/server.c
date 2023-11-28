#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define SERVER_PORT 5000  // Define the server port

int main(int argc, char *argv[]) {
    int listenfd, connfd;  // Socket file descriptors
    struct sockaddr_in serv_addr;  // Server address structure
    char from_client[1025];

    // Create a socket for the server
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));  // Clear the server address structure

    // Set server address parameters
    serv_addr.sin_family = AF_INET;  // Address family (IPv4)
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Accept any incoming interface
    serv_addr.sin_port = htons(SERVER_PORT);  // Server port

    // Bind the socket to the server address
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // Start listening for client connections
    listen(listenfd, 10);  // Backlog of 10 connections
    
    printf("Server running on port %d\n", SERVER_PORT);

    for(;;) {
        // Accept a connection from a client
        connfd = accept(listenfd, NULL, NULL);
        
        int recvLen = recv(connfd, from_client, 1024, 0);
        // Receive data from the client
        if (recvLen > 0) {
            from_client[recvLen] = '\0';  // ヌル文字を追加
            printf("File Requested: %s\n", from_client);
        } else {
            // エラーハンドリング
            printf("Receive error or connection closed by client\n");
        }

        // Close the connection and wait for a new one
        close(connfd);
        sleep(1);
    }
}
