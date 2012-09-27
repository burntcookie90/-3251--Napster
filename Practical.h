#ifndef PRACTICAL_H_
#define PRACTICAL_H_

#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>

#define FILENAME_LENGTH 25
#define FILE_ORIGIN_IP_LENGTH 16
#define LOAD_FACTOR 0.7
#define ARRAY_LENGTH 25

// Handle error with user msg
void DieWithUserMessage(const char *msg, const char *detail);
// Handle error with sys msg
void DieWithSystemMessage(const char *msg);
// Print socket address
void PrintSocketAddress(const struct sockaddr *address, FILE *stream);
// Test socket address equality
bool SockAddrsEqual(const struct sockaddr *addr1, const struct sockaddr *addr2);
// Create, bind, and listen a new TCP server socket
int SetupTCPServerSocket(const char *service);
// Accept a new TCP connection on a server socket
int AcceptTCPConnection(int servSock);
// Handle new TCP client
void HandleTCPClient(int clntSocket);
// Create and connect a new TCP client socket
int SetupTCPClientSocket(const char *server, const char *service);

enum sizeConstants {
  MAXSTRINGLENGTH = 128,
  BUFSIZE = 512,
};

//struct for the file. Contains the ip address of the file creator and the filename.
typedef struct file{
		char* origin_ip_address;
		char* filename;
} Napster_File;

extern int file_line;
extern char* filename;
extern Napster_File* list;

#endif // PRACTICAL_H_
