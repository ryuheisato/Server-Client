#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define SERVER_PORT 5000  // Define the server port
#define ROOT_DIR "./root/"

void *handle_request(void *connfd_ptr);

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
    fflush(stdout);

    for(;;) {
        // Accept a connection from a client
        connfd = accept(listenfd, NULL, NULL);
        
        pthread_t thread_id;
        int *connfd_ptr = malloc(sizeof(int));
        *connfd_ptr = connfd;
        if(pthread_create(&thread_id, NULL, handle_request, connfd_ptr) != 0) {
            perror("Could not create thread");
            free(connfd_ptr);
            continue;
        }

        // スレッドの終了を待機しない（デタッチ）
        pthread_detach(thread_id);

        sleep(1);
    }
}

void *handle_request(void *connfd_ptr) {
    int connfd = *((int*)connfd_ptr);
    free(connfd_ptr);  // ポインタを解放
    
    char from_client[1025];
    int recvLen = recv(connfd, from_client, 1024, 0);
    char buffer[BUFFER_SIZE] = {0};
    // Receive data from the client
    if (recvLen > 0) {
        from_client[recvLen] = '\0';  // ヌル文字を追加
        
        // 改行文字の削除
        char *newline = strchr(from_client, '\n'); // 改行文字を探す
        if (newline) {
            *newline = '\0'; // 改行文字をヌル文字に置き換える
        }
        
        printf("File Requested: %s\n", from_client);
        fflush(stdout);
        
        char file_path[2048];
        snprintf(file_path, sizeof(file_path), "%s%s", ROOT_DIR, from_client);
        printf("%s\n", file_path);
        fflush(stdout);
        
        FILE *file = fopen(file_path, "rb");
        if (file == NULL) {
            perror("File open error");
            return NULL;
        }

        // ファイルサイズを取得
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        // ファイルサイズを送信
        char file_size_str[20];
        sprintf(file_size_str, "%ld", file_size);
        send(connfd, file_size_str, strlen(file_size_str), 0);

        // flag
        const char* end_flag = "\n";
        send(connfd, end_flag, strlen(end_flag), 0);
        
        size_t total_sent = 0;
        size_t bytes_read;

        // ファイルの内容を送信
        while (total_sent < file_size) {
            bytes_read = fread(buffer, 1, sizeof(buffer), file);
            printf("%.*s", (int)bytes_read, buffer);
            send(connfd, buffer, bytes_read, 0);
            total_sent += bytes_read;
        }

        // char buffer[1024];
        // int bytesRead;
        // while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        //     send(connfd, buffer, bytesRead, 0);
        // }

        fclose(file);
        close(connfd);  // コネクションを閉じる
        return NULL;  // スレッド終了
    } else {
        printf("Receive error or connection closed by client\n");
    }
}
