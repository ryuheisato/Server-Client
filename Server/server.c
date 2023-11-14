// Include necessary headers for network programming
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define SERVER_PORT 5000  // Define the server port

int main(int argc, char *argv[]) {
    int listenfd, connfd;  // Socket file descriptors
    struct sockaddr_in serv_addr;  // Server address structure

    char sendBuff[1025];  // Buffer to hold outgoing data
    char from_client[1025];  // Buffer to receive data from client
    time_t ticks;  // For storing current time

    // Create a socket for the server
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));  // Clear the server address structure
    memset(sendBuff, '0', sizeof(sendBuff));  // Clear the send buffer

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

        // Receive data from the client
        recv(connfd, from_client, sizeof(from_client), 0);
        printf("%s", "Letter Received\n");

        // Send current time if first letter is 't'
        if(*from_client == 't') {
            ticks = time(NULL);
            int n = snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
            write(connfd, sendBuff, n);
        }

        // Send a greeting if first letter is 'n'
        if(*from_client == 'n') {
            strcpy(sendBuff, "Hello Ryuhei\n");
            send(connfd, sendBuff, strlen(sendBuff), 0);
        }

        // Send a specific integer if first letter is 'i'
        if(*from_client == 'i') {
            memcpy(sendBuff, &(int) { 201604543 }, sizeof(int));
            send(connfd, sendBuff, sizeof(int), 0);
        }

        // Close connection and exit if first letter is 'q'
        if(*from_client == 'q') {
            close(connfd);
            return 0;
        }

        // Close the connection and wait for a new one
        close(connfd);
        sleep(1);
    }
}
