#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>

#define SERVER_PORT 8234
#define MAXLEN 100

char * new_file = "RECEIVED_FILE.txt";

int checkDelimiter(char ch);
int checkWord(char * buff, int temp, int * prev);
int createSocket();
void connectSocket(int sockd);

int main(int argc, char * argv[]) {

  // Socket is created
  int sockd = createSocket();

  // Connection is established
  connectSocket(sockd);

  // define buffers
  char sendB[MAXLEN], recvB[MAXLEN];
  int st_fl = 0, fd;
  int wrd_cnt = 0, byte_cnt = 0, prev = 1;

  // Requesting for the file name
  printf("Enter File Name ---> ");

  // The file name from the user is read
  scanf("%[^\n]", sendB);

  // The file name to the server is sent
  send(sockd, (char * ) sendB, strlen(sendB), 0);

  while (1) {
    // The data from server is received
    int temp = recv(sockd, (char * ) recvB, sizeof(recvB), 0);

    // Check for force close or orderly shut down by the server when file is not present
    if ((temp < 0 && errno == ECONNRESET && st_fl == 0) || (temp == 0 && st_fl == 0)) {
      printf("* Connection closed by server.\n");
      printf("ERR 01: File Not Found");

      // The socket is closed
      close(sockd);

       // Exit with failure
      exit(EXIT_FAILURE);
    }

    // The new file is opened to write the data
    if (st_fl == 0)
      fd = open(new_file, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);

    // Check when the file is done transferring
    if ((temp < 0 && errno == ECONNRESET && st_fl == 1) || (temp == 0 && st_fl == 1)) {
      if (prev == 0)
        wrd_cnt += 1;
      printf("* Connection closed by server.\n");
      printf("* File Successfully received.\n");
      if(byte_cnt==101)
        byte_cnt=0,wrd_cnt=0;
      printf("* Size of file = %d bytes, no. of words = %d.\n", byte_cnt, wrd_cnt);
      close(sockd);
      close(fd);
      exit(EXIT_SUCCESS);
    }

    // Write the buffer received to the file
    write(fd, (const void * ) recvB, (size_t) temp);

    // The number of bytes is increased
    byte_cnt += temp;

    // The number of words is increased
    wrd_cnt += checkWord(recvB, temp, & prev);

    // Mark the condition for checking whether file is preseent.
    st_fl = 1;
  }

  // The socket is closed
  close(sockd);
}

int checkDelimiter(char ch) {
  if (ch == ' ' || ch == '\t' || ch == ':' || ch == ';' || ch == ',' || ch == '.' || ch=='\n' )
    return 1;
  return 0;
}

int checkWord(char * buff, int temp, int * prev) {
  int i = 0, words = 0;

  // The start is marked as non-delimiter
  if (!checkDelimiter(buff[0]))
    *prev = 0;

  while (i < temp) {
    // Go to the first delimiting character
    while (i < temp && !checkDelimiter(buff[i])) i++;

    if (i < temp && *prev == 0) words += 1; // Increase the word count by 1

    // Prev is reinitialised
    *prev = 0;

    // Go to the first non-delimiting char
    while (i < temp && checkDelimiter(buff[i])) i++;
  }

  // The end is marked as delimiter
  if (checkDelimiter(buff[i - 1]))
    *prev = 1;

  return words; //Words is returned
}

// Create socket
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
  remote.sin_port = htons(SERVER_PORT);
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
