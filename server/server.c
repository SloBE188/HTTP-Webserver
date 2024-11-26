#include <sys/socket.h>
#include "server.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>



int main(int argc, char** argv)
{
    struct sockaddr_in server_addr;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);    //SOCK_STREAM = TCP, AF_INET = IPv4
    if (server_fd < 0)
    {
        perror("socket creation failed\n");
        exit(EXIT_FAILURE);  
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;   //server accepts connections from any netfork interface
    server_addr.sin_port = 8080;
    
    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("couldnt bind the socket to the address and port in the struct\n");
        exit(EXIT_FAILURE);
    }

    int listening = listen(server_fd, 20);
    if(listening < 0)
    {
        perror("listening failed\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}