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
    int server_socket;
    int client_socket;
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;
    char buffer[BUFFER_SIZE];
    FILE *bc_pipe;
    int result;

    
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Server: Socket creation failed");
        exit(1);
    }

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);
    unlink(SOCKET_PATH);

    
    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Server: Bind failed");
        exit(1);
    }


    if (listen(server_socket, 5) < 0) {
        perror("Server: Listen failed");
        exit(1);
    }

    while(1) {

        int clen = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &clen);
        if (client_socket < 0) {
            perror("Server: Accept failed");
            exit(1);
        }


        memset(buffer, 0, BUFFER_SIZE);
        read(client_socket, buffer, BUFFER_SIZE - 1);
        printf("Server: Received expression '%s' from client\n", buffer);


        char bc_command[BUFFER_SIZE];
        snprintf(bc_command, sizeof(bc_command), "echo '%s' | bc", buffer);

      
        bc_pipe = popen(bc_command, "r");
        if (bc_pipe == NULL) {
            perror("Server: Failed to run bc");
            close(client_socket);
            continue;
        }

      
        memset(buffer, 0, BUFFER_SIZE);
        fread(buffer, 1, BUFFER_SIZE - 1, bc_pipe);
        pclose(bc_pipe);

        write(client_socket, buffer, strlen(buffer));

        close(client_socket);
    }


    close(server_socket);
    unlink(SOCKET_PATH);  
    return 0;
}

