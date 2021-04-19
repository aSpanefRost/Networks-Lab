#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define MAXLINE 10240

char * save_file = "RECEIVED_FILE.txt";
char * file_name;

void fileNotFoundError(char buffer[MAXLINE]);
void wordNoString(int wordn, char buffer[MAXLINE]);

int main(int argc, char * argv[]) {
  int sockfd_recv, sockfd_send;
  struct sockaddr_in servaddr, recvaddr;
  if (argc >= 2) {
    file_name = (char * ) malloc(100);
    strcpy(file_name, argv[1]);
  } else {
    file_name = (char * ) malloc(100);
    char d[] = "input.txt";
    strcpy(file_name, d);
  }

  // Creating socket file descriptor for receiving socket
  sockfd_recv = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd_recv < 0) {
    perror("receiving socket creation failed");
    exit(EXIT_FAILURE);
  }

  // Creating socket file descriptor for sending socket
  sockfd_send = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd_send < 0) {
    perror("sending socket creation failed");
    exit(EXIT_FAILURE);
  }

  /* Configuring Receiving Port */

  memset( & recvaddr, 0, sizeof(recvaddr));

  // Receiving port information
  recvaddr.sin_family = AF_INET;
  recvaddr.sin_port = htons(9342);
  recvaddr.sin_addr.s_addr = INADDR_ANY;

  // Binding the socket with the receiving address
  if (bind(sockfd_recv, (const struct sockaddr * ) & recvaddr,
      sizeof(recvaddr)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  printf("***** Receiving socket is ready *****\n");

  /* Configuring Server Address */

  memset( & servaddr, 0, sizeof(servaddr));

  // Server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(9234);
  servaddr.sin_addr.s_addr = INADDR_ANY;

  socklen_t len = sizeof(servaddr);

  char buffer[MAXLINE], content[MAXLINE], temp[MAXLINE];
  int n, words = 0;
  FILE * writeFile = NULL;
  fileNotFoundError(temp); // Not found error as string

  // Requesting the file from the server
  strcpy(content, file_name);
  sendto(sockfd_send, (const char * ) content, strlen(content), 0, (const struct sockaddr * ) & servaddr, len); // Sending the file name to the server
  printf("--> File request is sent\n");

  while (1) {
    // Waiting for message from server
    n = recvfrom(sockfd_recv, (char * ) buffer, MAXLINE, 0, NULL, NULL);
    buffer[n] = '\0';

    char target[MAXLINE];
    sscanf(buffer, "%s", target);

    // Checking if the message is not found error
    if (!strcmp(buffer, temp)) {
      printf("File Not Found\n");
      break;
    }

    // Checking if the message is HELLO
    else if (!strcmp(target, "HELLO")) {
      printf("<-- HELLO is received\n");
      writeFile = fopen(save_file, "w"); // Open the file in write mode
      printf("* Writing contents to \"%s\" \n", save_file);
      words += 1; // Start the word count
    }

    // Checking if the message is END
    else if (!strcmp(target, "END")) {
      printf("<-- END is received\n");
      fclose(writeFile); // Close file
      writeFile = NULL; // Set file pointer to NULL
      printf("* File is closed *\n");
      break;
    }

    // If the file is opened and ready for writing
    else if (writeFile != NULL) {
      printf("<-- WORD%d is Received\n", words);
      fprintf(writeFile, "%s", buffer); // Write the word to file
      printf("* Word%d is written  in file *\n", words);
      words += 1; // Increase word count
    }

    // If HELLO is not received
    else {
      printf("%s\n", buffer);
      exit(1);
    }

    // If file is opened and ready to write in, request for the next word
    if (writeFile != NULL) {
      wordNoString(words, content); // Generating the word request
      sendto(sockfd_send, (const char * ) content, strlen(content), 0,
        (const struct sockaddr * ) & servaddr, len); // Sending the request to the server
      printf("--> Request for Word%d is sent\n", words);
    }
  }

  // Closing the socket pointers
  close(sockfd_send);
  close(sockfd_recv);
  return 0;
}

// Generating the file not found error message
void fileNotFoundError(char buffer[MAXLINE]) {
  char temp[MAXLINE] = "FILE_NOT_FOUND";
  int len = strlen(file_name);
  strncat(temp, file_name, len);
  strcpy(buffer, temp);

}

// Generating the requesting word message which is formed from concatenating the word number to the 'WORD'
void wordNoString(int wordn, char buffer[MAXLINE]) {
  char temp[MAXLINE] = "WORD", temp1[100];

  // Converting the number to string
  sprintf(temp1, "%d", wordn);
  strncat(temp, temp1, strlen(temp1));
  strcpy(buffer, temp);
}
