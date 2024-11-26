#include <sys/socket.h>
#include "server.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>


char* load_file(const char* filename, long* filesize)
{
    FILE* filepointer = fopen(filename, "r");
    if (filepointer == NULL)
    {
        perror("couldn't open file");
        return NULL;
    }

    //get filesize fseek->sets filepointer to the end, ftell->gives curr position, rewind->sets filepointer to the start
    fseek(filepointer, 0, SEEK_END);
    *filesize = ftell(filepointer);
    rewind(filepointer);

    char* buffer = malloc(*filesize + 1);
    if (buffer == NULL)
    {
        perror("allocation failed");
        fclose(filepointer);
        return NULL;
    }

    fread(buffer, 1, *filesize, filepointer);
    buffer[*filesize] = '\0'; // null terminating string
    fclose(filepointer);

    return buffer;
}

// function for getting the mime type
const char* get_mime_type(const char* path)
{
    const char* ext = strrchr(path, '.');
    if (ext == NULL)
        return "application/octet-stream"; // fallback

    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";

    return "application/octet-stream"; // fallback
}



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
    if(listen(tcp_socketfd, 1000) < 0)
    {
        perror("failed to listen");
        return;
    }

    char buf[1024];

    struct sockaddr_in client_socket;

    //int count = 0;
    while(1)
    {
        /* Await a connection on socket FD.
        When a connection arrives, open a new socket to communicate with it,
        set *ADDR (which is *ADDR_LEN bytes long) to the address of the connecting
        peer and *ADDR_LEN to the address's actual length, and return the
        new socket's descriptor, or -1 for errors.

        This function is a cancellation point and therefore not marked with
        __THROW.  */
        socklen_t addr_len = sizeof(client_socket);
        int acceptfd = accept(tcp_socketfd, (struct sockaddr*)&client_socket, &addr_len);
        if (acceptfd < 0)
        {
            perror("accept failed\n");
            continue;
        }
        //count++;
        printf("Connected client:\n");
        printf("IP Address: %s\n", inet_ntoa(client_socket.sin_addr));
        printf("Port: %u\n", ntohs(client_socket.sin_port));

        socklen_t addr_len_client = sizeof(struct sockaddr);

        //int clientinfo = getpeername(acceptfd, (struct sockaddr*)&client_socket, &addr_len_client);
        //printf("Client IP-Adresse: %s\n Client Port Nummer: %s\n", inet_ntoa(client_socket.sin_addr), ntohs(client_socket.sin_port));
        
        char buf[1024];
        uint32_t return_value_read = read(acceptfd, buf, sizeof(buf) - 1);   //returns number of bytes that where loaded into the buffer
        if (return_value_read < 0)
        {
            perror("read failed\n");
            return;
        }

        buf[return_value_read] = '\0';
        printf("Request:\n%s\n", buf);

         // extract path from http request
        char method[8], path[1024];
        sscanf(buf, "%s %s", method, path);

        // if path = /, go to index.html (standardpath)
        if (strcmp(path, "/") == 0)
            strcpy(path, "/index.html");


        // create path if not standard lol
        char full_path[2048];
        snprintf(full_path, sizeof(full_path), "/home/slobe/HTTP-Webserver/server/website%s", path);

        // MIME-Typ ermitteln
        const char* mime_type = get_mime_type(full_path);

        // Datei laden
        long filesize;
        char* file_content = load_file(full_path, &filesize);
        if (file_content == NULL)
        {
            const char* error_response = "HTTP/1.1 404 Not Found\r\n"
                                         "Content-Type: text/plain\r\n\r\n"
                                         "404 Not Found";
            write(acceptfd, error_response, strlen(error_response));
            close(acceptfd);
            continue;
        }

        // send HTTP response and filecontent
        char response_header[256];
        snprintf(response_header, sizeof(response_header),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: %s\r\n"
                 "Content-Length: %ld\r\n\r\n",
                 mime_type, filesize);

        write(acceptfd, response_header, strlen(response_header));
        write(acceptfd, file_content, filesize);

        free(file_content);

       /*char* html_content = load_html_site("/home/slobe/HTTP-Webserver/server/website/testsite.html");
        if (html_content == NULL)
        {
            printf("Failed to load HTML site\n");
            close(acceptfd);
            continue;
        }

        char response_header[] = "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: text/html\r\n"
                                 "Content-Length: ";


        char content_length[16];
        snprintf(content_length, sizeof(content_length), "%ld", strlen(html_content));      //itoa >>> snprintf   

        char response_end[] = "\r\n\r\n";

        write(acceptfd, response_header, strlen(response_header));
        write(acceptfd, content_length, strlen(content_length));
        write(acceptfd, response_end, strlen(response_end));
        write(acceptfd, html_content, strlen(html_content));

        free(html_content); */
        close(acceptfd);

    }

    close(tcp_socketfd);

    return 0;
}