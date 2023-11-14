#include <sys/socket.h>  // Include necessary headers for socket programming
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>       // Standard input/output functions
#include <stdlib.h>      // Standard library functions
#include <string.h>      // String handling functions
#include <unistd.h>      // POSIX operating system API
#include <arpa/inet.h>   // Definitions for internet operations

int main(int argc, char *argv[]) {
    int sockfd;                       // Socket file descriptor
    struct sockaddr_in serv_addr;     // Server address structure
    char letter[2];                   // Array to store a single character input
    char recvBuff[1024];              // Buffer to store received data

    // Check for correct number of command line arguments
    if(argc != 2) {
        printf("\n Usage: %s <port> \n", argv[0]);
        return 1;
    }

    // Convert command line argument to integer for port number
    int port = atoi(argv[1]);
    if (port <= 0) {
        printf("\n Invalid port number \n");
        return 1;
    }

    // Prompt user for input
    printf("Enter a Character (t, n, i, q): ");
    scanf(" %c", letter);
    letter[1] = '\0';                 // Null terminate the string

    // Initialize server address to zero and set properties
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;               // Address family
    serv_addr.sin_port = htons(port);             // Set port number, converting to network byte order

    // Convert IPv4 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\n inet_pton error occurred\n");
        return 1;
    }

    // Create a socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    // Connect to the server
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    // Send the character to server
    send(sockfd, letter, strlen(letter), 0);
    printf("%s", "Letter Sent\n");
    
    // If character is 'q', quit
    if(*letter == 'q') {
        close(sockfd);
        return 0;
    }

    // Receive data from server
    ssize_t recvLen = recv(sockfd, recvBuff, sizeof(recvBuff), 0);
    
    // Print received data based on the command
    if(*letter == 'i') {
        printf("%d", *(int *) recvBuff); // If command is 'i', interpret as integer
    } else {
        printf("%.*s", (int) recvLen, recvBuff); // Otherwise, print as string
    }

    close(sockfd);  // Close the socket
    return 0;
}