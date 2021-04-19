#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define MAXLINE 10240

void fileNotFoundError(char * file, char buffer[MAXLINE]);
int numberDecode(char buffer[MAXLINE]);
void readWord(FILE * filePointer, int wordNum, char content[MAXLINE]);

int main() {
  int sockfd_recv, sockfd_send;
  struct sockaddr_in servaddr, cliaddr;

  /* Creating socket files */

  // Creating socket file descriptor for receiving
  sockfd_recv = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd_recv < 0) {
    perror("sending socket creation failed");
    exit(EXIT_FAILURE);
  }

  // Creating socket file descriptor for sending
  sockfd_send = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd_send < 0) {
    perror("receiving socket creation failed");
    exit(EXIT_FAILURE);
  }

  /* Configuring server address */

  // Creating memory for the server address
  memset( & servaddr, 0, sizeof(servaddr));

  // Filling in the details of the server
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(9234);

  // Binding the socket with the server address
  if (bind(sockfd_recv, (const struct sockaddr * ) & servaddr,
      sizeof(servaddr)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  /* Configuring client address */

  // creating memory for the client address
  memset( & cliaddr, 0, sizeof(cliaddr));

  // Filling in the details of the client
  cliaddr.sin_family = AF_INET;
  cliaddr.sin_addr.s_addr = INADDR_ANY;
  cliaddr.sin_port = htons(9342);

  socklen_t len = sizeof(cliaddr);
  printf("***** Server Running *****\n");

  char buffer[MAXLINE], content[MAXLINE];
  int n, run = 1;
  FILE * filePointer = NULL;

  // Keep running the server
  while (run) {
    // Waiting for request from client.
    n = recvfrom(sockfd_recv, (char * ) buffer, MAXLINE, 0, NULL, NULL);
    buffer[n] = '\0';

    // Decoding the request
    int wordReq = numberDecode(buffer);

    // Considering file request
    if (wordReq < 0 || filePointer == NULL) {
      // If no open file exists, try opening it
      if (filePointer == NULL) {
        filePointer = fopen(buffer, "r"); // Opening file in read mode

        if (filePointer == NULL)
          fileNotFoundError(buffer, content); // If no file exists, geenrating file not found error

        else {
          fscanf(filePointer, "%s", content); // Reading the first word into the buffer
          char a[] = "HELLO";
          if (strcmp(a, content)) {
            printf("Wrong File Format\n");
            memset(content, 0, sizeof(content));
            strcpy(content, "Wrong File Format");
          }
        }

      }

      // If already a file is open
      else {
        printf("* A file request already is under process *\n");
        continue;
      }
    }

    // If it is a word request
    else {
      printf("<-- Request for WORD%s was recieved \n", & buffer[4]);

      // Reading the ith word
      readWord(filePointer, atoi( & buffer[4]), content);

      // When encountering END, closing file after comparing the trimmed string
      char target[MAXLINE];
      sscanf(content, "%s", target);
      if (!strcmp(target, "END")) {
        fclose(filePointer);
        filePointer = NULL;
      }
    }

    // Sending the contents in buffer to client
    sendto(sockfd_send, (const char * ) content, strlen(content), 0, (const struct sockaddr * ) & cliaddr, len);
    printf("--> The data for the request was sent\n");
  }

  close(sockfd_send);
  close(sockfd_recv);
  return 0;
}

// Generating file not found error
void fileNotFoundError(char * file, char buffer[MAXLINE]) {
  char temp[MAXLINE] = "FILE_NOT_FOUND";
  int len = strlen(file);
  strncat(temp, file, len);
  strcpy(buffer, temp);
}

// Decoding the request
int numberDecode(char buffer[MAXLINE]) {
  char * comp = "WORD";
  int check = 1, i = 0;

  // Checking if the first 4 letters match the string WORD
  for (; i < strlen(comp); i += 1) {
    if (comp[i] != buffer[i]) {
      check = 0;
      break;
    }
  }

  // Returning -1 if it is not a proper word request
  if (!check || !atoi( & buffer[4]))
    return -1;

  // Else returning the word number
  return atoi( & buffer[4]);
}

// Reading the ith word in the file
void readWord(FILE * filePointer, int wordNum, char content[MAXLINE]) {
  int i = 0;
  char temp, temp1;

  rewind(filePointer); // Bringing the pointer to the start
  for (; i <= wordNum; i += 1)
    fscanf(filePointer, "%s", content); // Reading the ith word

  // Appending all newline and space characters
  while (fscanf(filePointer, "%c", & temp) != EOF) {
    if (temp == ' ' || temp == '\n' || temp == '\t')
      strncat(content, & temp, 1);
    else
      break;
  }
  fseek(filePointer, -1, SEEK_CUR); // Rewind to the last read non whitespace character
}
