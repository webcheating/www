#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "tui.h"

int main(){

    int port = 80;
    printf("[*] enter port: ");
    scanf("%d", &port);
    printf("\n[*] htons port: %u\n", htons(port));

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("[x] socket");
        return 1;
    } else{
        printf("[*] socket initialization success. returned %u\n", sock);
    }
    struct sockaddr_in addr = {
        AF_INET, 
        htons(port), 
        0
    };

    int bind_status;
    do{ 
        bind_status = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
        if(bind_status != 0){
            perror("[x] bind error");
            sleep(5);
        }
    } while(bind_status != 0);
    printf("[*] bind success. returned %u\n", bind_status);
    
    if(listen(sock, 20) < 0){
        perror("[x] listen error");
        return 1;
    } else{
       printf("[+] all done. listening on port %u\n", port); 
    }
    
    int client_count = 0;
    while(1){ 
        //printf("\n----------------- [ client %u ] -----------------\n", client_count);
        int client = accept(sock, 0, 0);
        char buf[60] = {0};
        int req = recv(client, buf, sizeof(buf), 0);
        //recv(client, buf, 256, 0); 
        if(req <= 0){
            close(client);
            continue;
        }
        printf("\n----------------- [ client %u ] -----------------\n", client_count);
        buf[req] = 0;

        // extracting requested path from HTTP request (updated)
        char method[16], path[256], version[16];
        char *first_line = strstr(buf, "\r\n");
        if(!first_line) continue;
        *first_line = 0;

        if(sscanf(buf, "%15s %255s %15s", method, path, version) == 3){
            printf("[*] %s %s %s", method, path, version);
        } else{
            perror("[x] error while parsing HTTP request"); 
            close(client);
            continue;
        }

        /*
        // extracting requested path from HTTP request 
        //
        char *http_start = strchr(buf, ' '); // determine HTTP method, since first line in request looks like: 'GET /requested/path HTTP/1.1' and there is always space between HTTP method and reqeusted path
        //if(!http_start) return -1;
        if(!http_start){
            close(client);
            continue;
        }
        //printf("\n[*] http_start: \n%s", http_start); 
        http_start += 2; // to remove '/' from path
        char *http_end = strchr(http_start, ' '); // determine requested path, since there is space between supplied path and HTTP version (even when path contains spaces, since they will be url-encoded)
        //if(!http_end) return -1;
        if(!http_end){
            close(client);
            continue;
        }
        //printf("\n[*] http_end: \n%s", http_end);
        
        // to remove all parts of HTTP request except the requested path
        size_t len = http_end - http_start;
        char path[256];
        if(len >= sizeof(path)) len = sizeof(path) - 1;
        memcpy(path, http_start, len); // copying to separate buffer, to keep the original request(buf[])
        path[len] = 0;
        */
        

        //printf("\n\n[*] output: \n%s", buf);
        printf("\n\n[+] extracted path: %s", path);

        // send message on a socket 
        char *response_buf = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 18\r\n\r\n<h1>hiii :333</h1>"; // building HTTP response, note that double '\r\n' before HTTP body is neccessary (read about CRLF)
        printf("\n[+] generated response: \n%s", response_buf);
        send(client, response_buf, strlen(response_buf), 0);
        printf("\n------------------------------------------------", client_count);
  
        // to close all created file descriptors properly
        // 
        // actually they will close automatically after program has finished running, but there may be some errors.
        // additionally, if you have multiple client file descriptors open, you may run out of limits for open files on your system
        close(client);
        //printf("[+] done, server client number %u", client_count);
        client_count++;
    }
    close(sock);
    return 0;
}
