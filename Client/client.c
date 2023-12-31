#include <sys/socket.h>  // Include necessary headers for socket programming
#include <netinet/in.h>
#include <stdio.h>       // Standard input/output functions
#include <stdlib.h>      // Standard library functions
#include <string.h>      // String handling functions
#include <unistd.h>      // POSIX operating system API
#include <arpa/inet.h>   // Definitions for internet operations

int main(int argc, char *argv[]) {
    int sockfd;                       // Socket file descriptor
    struct sockaddr_in serv_addr;     // Server address structure
    char filename[1024];              // Buffer to store received data

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
    printf("Enter the filename: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0'; // 改行文字を削除

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
    send(sockfd, filename, strlen(filename), 0);
    printf("File Sent\n");
    
    // ファイルサイズの読み取り
    char file_size_str[20];
    int valread_size = 0;
    while (1) {
        int bytes_read = read(sockfd, file_size_str + valread_size, 1);
        if (bytes_read <= 0) {
            break;
        }
        valread_size += bytes_read;
        if (file_size_str[valread_size - 1] == '\n') {
            break;
        }
    }
    file_size_str[valread_size - 1] = '\0';
    long file_size = atol(file_size_str);
    printf("File size: %ld bytes\n", file_size);
    
    // ファイルの内容を受信
    char *buffer = malloc(file_size + 1);
    long total_bytes_read = 0;
    while (total_bytes_read < file_size) {
        int bytes_read = read(sockfd, buffer + total_bytes_read, file_size - total_bytes_read);
        if (bytes_read < 0) {
            perror("Read error");
            break;
        }
        total_bytes_read += bytes_read;
    }
    buffer[total_bytes_read] = '\0';
    printf("Received file content:\n%s\n", buffer);

    free(buffer);
    close(sockfd);
    return 0;
}