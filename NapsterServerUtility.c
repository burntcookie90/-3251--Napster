#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "Practical.h"

#define DEBUG 1
static const int MAXPENDING = 5; // Maximum outstanding connection requests
char* state;

int SetupTCPServerSocket(const char *service) {
	// Construct the server address structure
	struct addrinfo addrCriteria;                   // Criteria for address match
	memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
	addrCriteria.ai_family = AF_UNSPEC;             // Any address family
	addrCriteria.ai_flags = AI_PASSIVE;             // Accept on any address/port
	addrCriteria.ai_socktype = SOCK_STREAM;         // Only stream sockets
	addrCriteria.ai_protocol = IPPROTO_TCP;         // Only TCP protocol

	struct addrinfo *servAddr; // List of server addresses
	int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
	if (rtnVal != 0)
		DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

	int servSock = -1;
	for (struct addrinfo *addr = servAddr; addr != NULL; addr = addr->ai_next) {
		// Create a TCP socket
		servSock = socket(addr->ai_family, addr->ai_socktype,
				addr->ai_protocol);
		if (servSock < 0)
			continue;       // Socket creation failed; try next address

		// Bind to the local address and set socket to listen
		if ((bind(servSock, addr->ai_addr, addr->ai_addrlen) == 0) &&
				(listen(servSock, MAXPENDING) == 0)) {
			// Print local address of socket
			struct sockaddr_storage localAddr;
			socklen_t addrSize = sizeof(localAddr);
			if (getsockname(servSock, (struct sockaddr *) &localAddr, &addrSize) < 0)
				DieWithSystemMessage("getsockname() failed");
			fputs("Binding to ", stdout);
			PrintSocketAddress((struct sockaddr *) &localAddr, stdout);
			fputc('\n', stdout);
			break;       // Bind and listen successful
		}

		close(servSock);  // Close and try again
		servSock = -1;
	}

	// Free address list allocated by getaddrinfo()
	freeaddrinfo(servAddr);

	return servSock;
}

int AcceptTCPConnection(int servSock) {
	struct sockaddr_storage clntAddr; // Client address
	// Set length of client address structure (in-out parameter)
	socklen_t clntAddrLen = sizeof(clntAddr);

	// Wait for a client to connect
	int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
	if (clntSock < 0)
		DieWithSystemMessage("accept() failed");

	// clntSock is connected to a client!

	fputs("[TCPServerUtility.c] Handling client ", stdout);
	PrintSocketAddress((struct sockaddr *) &clntAddr, stdout);
	fputc('\n', stdout);

	return clntSock;
}

void HandleTCPClient(int clntSocket, char* clientIP) {
	char buffer[BUFSIZE]; // Buffer for echo string

	if(DEBUG) printf("[NapsterServerUtility] Handling TCP Client %s\n",clientIP);
	// Receive message from client
	ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFSIZE, 0);
	buffer[numBytesRcvd] = '\0';

	if (numBytesRcvd < 0)
		DieWithSystemMessage("recv() failed");
	if(DEBUG) printf("[NapsterServerUtility] Received: %s\n",buffer);

	state = malloc(sizeof(char*));
	strncpy(state,buffer,1);
	char* idleState = "0";
	char* addState = "1";
	char* listState = "2";


	//ADDSTATE

	// Send received string and receive again until end of stream
	while (numBytesRcvd > 0) { // 0 indicates end of stream

		if(strncmp(addState,state,1)==0){
			memmove(buffer,buffer+1,strlen(buffer));
			buffer[strlen(buffer)] = 0;
			if(DEBUG) printf("[NapsterServerUtility] Line in %s: %d\n",filename,file_line);
			if(DEBUG) printf("[NapsterServerUtility] Previous file line: %s\n",list[file_line-1].filename);
			list[file_line].filename = buffer;	//copy the buffer into the struct
			list[file_line].origin_ip_address = clientIP; //copy the ip address into the struct
			if(DEBUG) printf("[NapsterServerUtility] Loaded filename %s into array\n",list[file_line].filename);

			//write the data to the file
			FILE *fp;
			if((fp = fopen(filename,"a+"))){
				fprintf(fp,"%s %s\n",list[file_line].origin_ip_address,list[file_line].filename);
				printf("[NapsterServerUtility] 127.0.0.1 %s\n",list[file_line].filename);
				file_line++;
				fclose(fp);
			}
			else if(DEBUG) printf("Unable to open file!\n");

			if(DEBUG) printf("[NapsterServerUtility] Echo message back to client %s\n", list[file_line-1].filename);

			// Echo message back to client
			ssize_t numBytesSent = send(clntSocket, buffer, numBytesRcvd, 0);
			if(DEBUG) printf("[NapsterServerUtility] Message Echoed\n");
			if (numBytesSent < 0)
				DieWithSystemMessage("send() failed");
			else if (numBytesSent != numBytesRcvd)
				DieWithUserMessage("send()", "sent unexpected number of bytes");

			//			// See if there is more data to receive
			if(DEBUG) printf("[NapsterServerUtility] waiting for more data\n");
			numBytesRcvd = recv(clntSocket, buffer, BUFSIZE, 0);
			if(DEBUG) printf("[NapsterServerUtility] done waiting for more data\n");
			if (numBytesRcvd < 0)
				DieWithSystemMessage("recv() failed");
			state = idleState;
		}
		else if(strncmp(listState,state,1)==0){
			if(DEBUG) printf("[NapsterServerUtility] Listing files!\n");
			char length[BUFSIZE];
			int list_size = sizeof(list);
			sprintf(length,"%d",list_size);
			if(DEBUG) printf("[NapsterServerUtility] Data to Send: %d %s\n",list_size,length);

			//sending file size to client
			if(DEBUG) printf("[NapsterServerUtility] sending file size to client\n");
			ssize_t numBytesSent = send(clntSocket,length,strlen(length),0);

			if (numBytesSent < 0)
				DieWithSystemMessage("send() failed");
			else if (numBytesSent != numBytesRcvd)
				DieWithUserMessage("send()", "sent unexpected number of bytes");

			//recv ack
			char ackbuffer[BUFSIZE];
			ssize_t numBytesAck1 = recv(clntSocket,ackbuffer,BUFSIZE, 0);

			if(numBytesAck1 < 0){
				DieWithSystemMessage("recv() failed");
			}
			char* file_list;
			file_list = malloc(file_line*sizeof(char*));
//			if(DEBUG) printf("[NapsterServerUtility] %s %s\n",list[0].origin_ip_address,list[2].filename);
			sprintf(file_list,"%s %s\n",list[0].origin_ip_address,list[0].filename);
			int i;
			for(i = 1; i<file_line;i++){
				if(DEBUG) printf("[NapsterServerUtility] %d %s %s\n",i,list[i].origin_ip_address,list[i].filename);
				strcat(file_list,list[i].origin_ip_address);
				strcat(file_list," ");
				strcat(file_list,list[i].filename);
				strcat(file_list,"\n");
//				sprintf(file_list,"%s %s\n",list[i].origin_ip_address,list[i].filename);
			}

			if(DEBUG) printf("[NapsterServerUtility]\n%s\n",file_list);
			if(DEBUG) printf("[NapsterServerUtility] sending file to client\n");
			//sending file to client
			ssize_t numBytesSentList = send(clntSocket,list,strlen(file_list),0);
			if(numBytesSentList<0)
				DieWithSystemMessage("send() failed");
			if(DEBUG) printf("[NapsterServerUtility] filelist sent\n");
			//			FILE* fp;
			//			if((fp=fopen(filename,"r"))){
			//				fclose(fp);
			//			}
			//			else
			//				DieWithSystemMessage("[NapsterServerUtility] file cannot be opened for listing files");

			state = idleState;
		}
		//		printf("[NapsterServerUtility] Leaving HandleTCPClient()\n");
	}

	//  close(clntSocket); // Close client socket
}
