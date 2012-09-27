#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"

#define DEBUG 1
static const int MAXPENDING = 5; // Maximum outstanding connection requests

int file_line;
char* filename;
Napster_File* list;

//char* filename = "napster_filelist.txt";
int main(int argc, char *argv[]) {
	char file_origin_ip[FILE_ORIGIN_IP_LENGTH];
	char input_filename[FILENAME_LENGTH];

	filename = "napster_filelist.txt";

	//Check if file exists, if so load it, otherwise create it.
	FILE *fp;
	list = malloc(ARRAY_LENGTH*sizeof(Napster_File));
	file_line = 0;

	if ((fp = fopen(filename, "r"))) {
		printf("filelist exists \n");

		while(fscanf(fp,"%s %s",file_origin_ip,input_filename) != EOF){
//			if(DEBUG) printf("Entered while loop in echo server %s %s\n",file_origin_ip,input_filename);
			if(DEBUG) printf("[NapsterServer] %d: Loaded %s into array from origin %s\n",file_line,list[file_line].filename,list[file_line].origin_ip_address);

			//store filename in array
//			list[file_line].filename = input_filename;
			list[file_line].filename = malloc(sizeof(char*));
			strcpy(list[file_line].filename,input_filename);
//			if(DEBUG) printf("Loaded filename %d %s\n",file_line,list[file_line].filename);

			//store origin ip address in array
//			list[file_line].origin_ip_address = file_origin_ip;
			list[file_line].origin_ip_address = malloc(sizeof(char*));
			strcpy(list[file_line].origin_ip_address,file_origin_ip);
//			if(DEBUG) printf("Loaded ip address %d %s\n",file_line,list[file_line].origin_ip_address);

			if(DEBUG) printf("[NapsterServer] %d: Loaded %s into array from origin %s\n",file_line,list[file_line].filename,list[file_line].origin_ip_address);
			file_line++;
		}

		if(DEBUG){
			int i;
			for(i = 0; i< file_line;i++){
//				printf("[NapsterServer] %d %s %s\n",i,list[i].origin_ip_address,list[i].filename);
				printf("[NapsterServer] %d: Loaded %s into array from origin %s\n",i,list[i].filename,list[i].origin_ip_address);
			}
		}
		fclose(fp);
		printf("closing file\n");
	}
	else{
		printf("File list does not exist! Creating filelist under name: %s\n",filename);
		fp = fopen(filename, "w");
		fclose(fp);
	}

	if (argc != 2) // Test for correct number of arguments
		DieWithUserMessage("Parameter(s)", "<Server Port>");

	in_port_t servPort = atoi(argv[1]); // First arg:  local port

	// Create socket for incoming connections
	int servSock; // Socket descriptor for server
	if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithSystemMessage("socket() failed");

	// Construct local address structure
	struct sockaddr_in servAddr;                  // Local address
	memset(&servAddr, 0, sizeof(servAddr));       // Zero out structure
	servAddr.sin_family = AF_INET;                // IPv4 address family
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
	servAddr.sin_port = htons(servPort);          // Local port

	// Bind to the local address
	if (bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
		DieWithSystemMessage("bind() failed");

	// Mark the socket so it will listen for incoming connections
	if (listen(servSock, MAXPENDING) < 0)
		DieWithSystemMessage("listen() failed");

	for (;;) { // Run forever
		struct sockaddr_in clntAddr; // Client address
		// Set length of client address structure (in-out parameter)
		socklen_t clntAddrLen = sizeof(clntAddr);

		// Wait for a client to connect
		int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
		if (clntSock < 0)
			DieWithSystemMessage("accept() failed");

		// clntSock is connected to a client!

		char clntName[INET_ADDRSTRLEN]; // String to contain client address
		if (inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntName,
				sizeof(clntName)) != NULL)
			printf("[NapsterServer] Handling client %s/%d\n", clntName, ntohs(clntAddr.sin_port));
		else
			puts("Unable to get client address");

		printf("[NapsterServer] Handling tcp client\n");
		HandleTCPClient(clntSock, clntName);
	}
	// NOT REACHED
}
