#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "unix_socket"
#define BUFFER_SIZE 256

int main() {
    int client_socket;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];
    int connection_result;


    client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Client: Socket creation failed");
        exit(1);
    }


    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);


    connection_result = connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (connection_result == -1) {
        perror("Client: Connect failed");
        exit(1);
    }


    printf("Enter an arithmetic expression (e.g., 2+2): ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;

    
    write(client_socket, buffer, strlen(buffer) + 1);

    memset(buffer, 0, BUFFER_SIZE);
    read(client_socket, buffer, BUFFER_SIZE - 1);


    printf("Client: The result is %s\n", buffer);


    close(client_socket);
    return 0;
}

