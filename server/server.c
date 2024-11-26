#include <sys/socket.h>
#include "server.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <stdlib.h>



int main(int argc, char** argv)
{

    struct sockaddr_in server_socket;
    int tcp_socketfd;

    tcp_socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(tcp_socketfd < 0)
    {
        perror("failed creating the tcp socket");
        return;
    }

    int opt = 1;
    if (setsockopt(tcp_socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt failed");
        return 1;
    }

    server_socket.sin_family = AF_INET;
    server_socket.sin_addr.s_addr = htonl(INADDR_ANY);
    server_socket.sin_port = htons(8080);

    if(bind(tcp_socketfd, (struct sockaddr*)&server_socket, sizeof(struct sockaddr)) < 0)
    {
        perror("failed to bind\n");
        return;
    }
    if(listen(tcp_socketfd, 64) < 0)
    {
        perror("failed to listen");
        return;
    }
    char* buf[1024];
    char response[] = "HTTP/1.0 200 OK\r\n"
                "Server: webserver-c\r\n"
                "Content-type: text/html\r\n\r\n"
                "<html>hallo ursiiii</html>\r\n";
    while(1)
    {
        /* Await a connection on socket FD.
        When a connection arrives, open a new socket to communicate with it,
        set *ADDR (which is *ADDR_LEN bytes long) to the address of the connecting
        peer and *ADDR_LEN to the address's actual length, and return the
        new socket's descriptor, or -1 for errors.

        This function is a cancellation point and therefore not marked with
        __THROW.  */
        socklen_t addr_len = sizeof(struct sockaddr);
        int acceptfd = accept(tcp_socketfd, (struct sockaddr*)&server_socket, &addr_len);
        if (acceptfd < 0)
        {
            perror("accept failed\n");
            return;
        }
        printf("accept succesfull\n");

        uint32_t return_value = read(acceptfd, buf, sizeof(buf));   //number of bytes that where loaded into the buffer
        if (return_value < 0)
        {
            perror("read failed\n");
            return;
        }

        uint32_t response_value = write(acceptfd, &response, sizeof(response));     //number of byter written
        

        close(acceptfd);

    }



    return 0;
}