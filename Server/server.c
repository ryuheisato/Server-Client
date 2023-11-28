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
#define ROOT_DIR "./root/"

void handle_request(int connfd);

int main(int argc, char *argv[]) {
    int listenfd, connfd;  // Socket file descriptors
    struct sockaddr_in serv_addr;  // Server address structure

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
        handle_request(connfd);

        // Close the connection and wait for a new one
        close(connfd);
        sleep(1);
    }
}

void handle_request(int connfd) {
    char from_client[1025];
    int recvLen = recv(connfd, from_client, 1024, 0);
    // Receive data from the client
    if (recvLen > 0) {
        from_client[recvLen] = '\0';  // ヌル文字を追加
        
        // 改行文字の削除
        char *newline = strchr(from_client, '\n'); // 改行文字を探す
        if (newline) {
            *newline = '\0'; // 改行文字をヌル文字に置き換える
        }
        
        printf("File Requested: %s\n", from_client);
        
        char file_path[2048];
        snprintf(file_path, sizeof(file_path), "%s%s", ROOT_DIR, from_client);
        printf("%s\n", file_path);
        fflush(stdout);
        
        FILE *file = fopen(file_path, "rb");
        if (file == NULL) {
            perror("File open error");
            return;
        }

        // ファイルサイズを取得
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        // ファイルサイズを送信
        char file_size_str[20];
        sprintf(file_size_str, "%ld\n", file_size);
        send(connfd, file_size_str, strlen(file_size_str), 0);

        // flag
        const char* end_flag = "\nEND\n";
        send(connfd, end_flag, strlen(end_flag), 0);

        // ファイルの内容を送信
        char buffer[1024];
        int bytesRead;
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            send(connfd, buffer, bytesRead, 0);
        }

        fclose(file);
    } else {
        printf("Receive error or connection closed by client\n");
    }
}
