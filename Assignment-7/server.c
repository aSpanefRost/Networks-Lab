#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <fcntl.h>

#define PORT 42042
#define MAX_SIZE 1024
#define BLOCKSIZE 20

int createSocket();
void bindSocket(int sockd);

int main() {
    int tcpfd, filesize, l;

    // char arrays for filename and buffer
    char buffer[MAX_SIZE], filename[MAX_SIZE];
    socklen_t len;

    // variables to store requesting client addresses
    socklen_t client_len;
    struct sockaddr_in cli_addr;
    struct stat st;

    // creates a socket for communication
    tcpfd = createSocket();
    bindSocket(tcpfd);

    // queue upto 5 requesting connections
    listen(tcpfd, 5);
    printf("===> Listening \n\n");
    while (1) {
        client_len = sizeof(cli_addr);

        // accept incoming connections
        int newsockfd = accept(tcpfd, (struct sockaddr * ) & cli_addr, & client_len);
        if (newsockfd != -1) {

            // receive filename from client
            l = recv(newsockfd, filename, MAX_SIZE, 0);
            int fp = open(filename, O_RDONLY);
            if (fp >= 0) {

                // sending length of file
                strcpy(buffer, "L");
                send(newsockfd, buffer, 1, 0);
                stat(filename, & st);
                filesize = st.st_size;
                send(newsockfd, & filesize, sizeof(filesize), 0);

                // reading from file and sending to client
                int no_of_blocks = 0;
                while ((l = read(fp, buffer, BLOCKSIZE)) > 0) {
                    no_of_blocks++;
                    send(newsockfd, buffer, l, 0);
                }
                int last_block_size = filesize % BLOCKSIZE;

                // printing file details
                printf("File size is:       %d\n", filesize);
                printf("Blocksize is:       %d\n", BLOCKSIZE);
                printf("No of blocks is:    %d\n", no_of_blocks);
                if (last_block_size != 0 || filesize == 0)
                    printf("Last block size is: %d\n\n", last_block_size);
                else
                    printf("Last block size is: %d\n\n", BLOCKSIZE);
            } else {
                // if file is not present in directory of server
                strcpy(buffer, "E");
                send(newsockfd, buffer, sizeof(buffer), 0);
            }

            // closing file descriptors
            close(fp);
            close(newsockfd);
        } else {
            // error in accepting connection
            perror("Accept error\n");
            exit(0);
        }
    }

    return 0;
}

// Socket is created and its id is returned
int createSocket() {

    // Socket is created with data stream packets
    int sockd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockd < 0) {
        printf("---> Error creating socket\n");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(sockd, SOL_SOCKET, SO_REUSEADDR, & (int) {
            1
        }, sizeof(int)) < 0)
        perror("---> setsockopt(SO_REUSEADDR) failed");
    return sockd;
}

// Socket is binded to the specified address
void bindSocket(int sockd) {
    struct sockaddr_in server;

    // The address of the server is configured
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind it
    int temp = bind(sockd, (const struct sockaddr * ) & server, sizeof(server));
    if (temp < 0) {
        printf("---> Binding failed\n");
        exit(EXIT_FAILURE);
    }
}
