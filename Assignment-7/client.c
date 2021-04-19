#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
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
#include <sys/stat.h>

#define PORT 42042
#define MAX_SIZE 1024
#define BLOCK_SIZE 20

char * new_file = "RECEIVED_FILE.txt";

int createSocket();
void connectSocket(int sockd);

int main() {
    int sockfd1, filesize, fp;

    // char arrays for filename and buffer
    char buffer[MAX_SIZE], filename[MAX_SIZE];
    int n, len;

    // creates a socket for communication
    sockfd1 = createSocket();
    int option = 1;
    setsockopt(sockfd1, SOL_SOCKET, SO_REUSEADDR, & option, sizeof(option));
    connectSocket(sockfd1);

    // taking file name as input from user
    printf("Enter file name: ");
    scanf("%[^\n]s", filename);

    // sends filename to server
    send(sockfd1, (char * ) filename, strlen(filename) + 1, 0);

    for (int i = 0; i < MAX_SIZE; i++) buffer[i] = '\0';

    // receives 'L' or 'E'
    len = recv(sockfd1, buffer, 1, MSG_WAITALL);

    if (buffer[0] != 'E') {
        // receive filesize
        len = recv(sockfd1, & filesize, 4, MSG_WAITALL);

        // open new file for writing
        fp = open(new_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        // receiving file in blocks and writing in new file
        int complete_blocks = filesize / BLOCK_SIZE, last_block_size = filesize % BLOCK_SIZE;
        for (int i = 0; i < complete_blocks; i++) {
            len = recv(sockfd1, buffer, BLOCK_SIZE, MSG_WAITALL);
            write(fp, buffer, len);
        }

        // recieving last block
        len = recv(sockfd1, buffer, last_block_size, MSG_WAITALL);
        write(fp, buffer, len);

        // printing file details
        int no_of_blocks = complete_blocks + ((last_block_size == 0) ? 0 : 1);
        printf("\nThe file transfer is successful!\n\n");
        printf("File size is:       %d\n", filesize);
        printf("Blocksize is:       %d\n", BLOCK_SIZE);
        printf("No of blocks is:    %d\n", complete_blocks + ((last_block_size == 0) ? 0 : 1));
        if (last_block_size != 0 || filesize == 0)
            printf("Last block size is: %d\n", last_block_size);
        else
            printf("Last block size is: %d\n", BLOCK_SIZE);
    } else {
        printf("File not found on server!\n");
    }
    // end connection if 'E'
    close(sockfd1);

    return 0;
}

int createSocket() {
    // Socket is created for requesting connection
    int rsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (rsocket < 0) {
        printf("* Socket creation failed");
        exit(EXIT_FAILURE);
    }
    return rsocket;
}
// try to connect to the remote server
void connectSocket(int sockd) {
    struct sockaddr_in remote;

    // setup the remote address
    remote.sin_family = AF_INET;
    remote.sin_port = htons(PORT);
    remote.sin_addr.s_addr = INADDR_ANY;

    // Connect to the remote host of the specified id
    int val = connect(sockd, (const struct sockaddr * ) & remote, sizeof(remote));

    // If socket creation is failed
    if (val == -1) {
        printf("* Connection failed\n");
        exit(EXIT_FAILURE);
    }
    printf("* Connection established\n");
}
