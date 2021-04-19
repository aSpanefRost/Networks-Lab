#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>

#define SERVER_PORT 8234
#define MAXLEN 100

void error(const char * msg);
int createSocket();
void bindSocket(int sockd);

int main() {
  while (1) {

     // socket is created
    int sockd = createSocket();

    // bind the socket
    bindSocket(sockd);

    // queue upto 3 requesting connections
    listen(sockd, 3);
    printf("===> Listening \n");

    // Variables to store the requesting client addresses
    struct sockaddr_in client;
    socklen_t sin_size = sizeof(client);
    char sendB[MAXLEN], recvB[MAXLEN];
    int sendSock = accept(sockd, (struct sockaddr * ) & client, (socklen_t * ) & sin_size); // Accept incoming connections

    // If connection fails
    if (sendSock < 0) {
      printf("---> Connection failed\n");
      continue;
    }

    // IP of the established connection is printed
    printf("---> Connection is established with %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    // Filename is received
    int n = recv(sendSock, (char * ) recvB, sizeof(recvB), 0);
    recvB[n] = '\0';
    printf("---> Request for file \"%s\" is received.\n", recvB);

     // The file is opened in read only mode
    int fd = open(recvB, O_RDONLY);

    // Check if file exists or not
    if (fd < 0) {
      printf("---> File not found.\n* Closing Connection.\n");
      // Closing the socket
      close(sendSock);
      close(sockd);
      continue;
    }
    int cnt = 0;

    while (1) {

       // read from the file descriptor
      int temp = read(fd, (void * ) sendB, 100);

      // EOF is reached
      if (temp == 0) {
        if(cnt == 0) {
          char st[] = "empty";
          printf("%s", st);
          temp=101;
          send(sendSock, (char * ) st, temp, 100);
        }
        //send(sendSock, (char * ) sendB, temp, 0);
        printf("---> File sent. Closing connection.\n");
        // The socket is closed
        close(sendSock);
        close(sockd);
        // The file descriptor is closed
        close(fd);
        break;
      }

      // The message in the buffer is sent to the client
      send(sendSock, (char * ) sendB, temp, 0);
      cnt++;
    }
    close(sendSock);
    close(sockd);
  }
}

void error(const char * msg) {
  perror(msg);
  exit(1);
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
    error("---> setsockopt(SO_REUSEADDR) failed");
  return sockd;
}

// Socket is binded to the specified address
void bindSocket(int sockd) {
  struct sockaddr_in server;

  // The address of the server is configured
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(SERVER_PORT);

  // Bind it
  int temp = bind(sockd, (const struct sockaddr * ) & server, sizeof(server));
  if (temp < 0) {
    printf("---> Binding failed\n");
    exit(EXIT_FAILURE);
  }
}
