#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"

#define DEBUG 1
#define WAITFORINPUT 0
#define PROMPT_STATE 1
#define QUITSTATE 2
#define ADDSTATE 3
#define LISTFILESTATE 4
#define DELETESTATE 5

int main(int argc, char* argv[]){
	char prompt[12] = "retspan>>: ";
	char *filename_in;
	int state = PROMPT_STATE;

	if (argc<2 || argc >3) {
		DieWithUserMessage("Parameter(s)", "<Server Address> [<Server Port>]");
	}

	char *servIP = argv[1]; 	//First arg: server IP address (dotted quad)

	//Third arg (optional): server port (numeric). 7 is well-known echo port
	in_port_t servPort = (argc == 3) ? atoi(argv[2]) : 7;

	//Create a reliable, stream socket using TCP
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock<0) {
		DieWithSystemMessage("socket() failed");
	}

	//Construct the server address structure
	struct sockaddr_in servAddr; //Server address
	memset(&servAddr, 0, sizeof(servAddr)); 	//Zero out structure
	servAddr.sin_family = AF_INET; 	//IPv4 address family

	//Convert address

	int rtnVal = inet_pton(AF_INET,servIP, &servAddr.sin_addr.s_addr);

	if (rtnVal==0) {
		DieWithUserMessage("inet_pton() failed", "invalid address string");
	}
	else if (rtnVal < 0) {
		DieWithSystemMessage("inet_pton() failed");
	}
	servAddr.sin_port = htons(servPort); 	//Server port

	//Establish the connection to the echo server
	if (connect(sock, (struct sockaddr *) & servAddr, sizeof(servAddr))<0) {
		DieWithSystemMessage("connect() failed");
	}

	fputc('\n', stdout); 	//Print a final linefeed

	//Begin the shell!
	printf("Welcome to Retspan!\nRevision 13.2:Missouri\n");
	while(1){
		char *command;
		char input[256];
		char *arg;
		//			char *arg2;
		char *tokenPtr;
		//			char *quit;
		//			quit = "quit";
		if(DEBUG) printf("\nCurrent State %d\n",state);
		if(state == PROMPT_STATE){
			printf("\n%s",prompt);
			state = WAITFORINPUT;
		}
		else if(state == WAITFORINPUT){	//wait for user input
			tokenPtr = strtok(fgets(input,256,stdin)," ");
			command = tokenPtr;
			tokenPtr = strtok(NULL, " ");
			arg = tokenPtr;
			//
			//			tokenPtr = strtok(NULL, " ");
			//			arg2 = tokenPtr;

			//check if user wants to quit
			if(strncmp(command,"quit",4)==0 || strncmp(command,"exit",4) == 0){
				printf("SAY BYE TO RETSPAN!!!!! IT LOVES YOUU\n");
				state = QUITSTATE;
			}
			else if(strncmp(command, "addfile",7)==0){ //check if user wants to add a file
				state = ADDSTATE;
			}
			else if(strncmp(command,"filelist",8)==0){ 	//check for list files
				state = LISTFILESTATE;
			}
		}
		else if(state == QUITSTATE){ //execute quit code
			close(sock);
			return 0;
		}
		else if(state == ADDSTATE){ //execute add file code
			if(arg != NULL){
				if(strncmp(arg,"-d",2)==0){
					state = DELETESTATE;
				}
				else{
					filename_in = malloc(255*sizeof(char));
					//filename_in = arg;
					arg[strlen(arg)-1] = 0;
					strcat(filename_in,"1");
					strcat(filename_in,arg);
					printf("[NapsterClient] ADDING %s BREH\n",filename_in);
					size_t filenameLen = strlen(filename_in); 	//Determine input length

					//	Send the string to the server
					ssize_t numBytes = send(sock, filename_in, filenameLen,0);

					if (numBytes < 0) {
						DieWithSystemMessage("send() failed");
					}
					else if (numBytes != filenameLen) {
						DieWithUserMessage("send()", "sent unexpected number of bytes");
					}
				/*/Receive the same string back from the server*/
					unsigned int totalBytesRcvd = 0; //Count o total bytes received
					fputs("[NapsterClient] Received file: ",stdout); 	//Setup to print the echoed string

					while(totalBytesRcvd < filenameLen){
						/*Receive up to the buffer size ( minus 1 to leave space for a null terminator ) bytes from the sender*/
						char buffer[BUFSIZE]; 	//I/0 Buffer
						numBytes = recv(sock, buffer, BUFSIZE -1, 0);
						if (numBytes < 0) {
							DieWithSystemMessage("recv() failed");
						}
						else if (numBytes == 0) {
							DieWithUserMessage("recv()", "connection closed prematurely");
						}

						totalBytesRcvd += numBytes; //Keep tally of total bytes
						buffer[numBytes]= '\0'; 	//Terminate the string!
						fputs(strcat(buffer,"\n"),stdout); 	//Print the echo buffer
					}
					/*send ack to the server for list size*/
					/*char list_ack[1] = "2";*/
					/*if(DEBUG) printf("[NapsterClient] sending ACK to server\n");*/
					/*int ack_len = strlen(list_ack);*/
					/*ssize_t numBytesAck = send(sock,list_ack,ack_len,0);*/
					/*if(DEBUG) printf("[NapsterClient] sent ack to server\n");*/
					/*if(numBytesAck <0)*/
						/*DieWithSystemMessage("send() failed");*/

					state=PROMPT_STATE;
				}
			}
		}
		else if (state == LISTFILESTATE){
			char* serv_state = malloc(sizeof(char*));
			serv_state = "2";
			printf("[NapsterClient]Listing files!\n");
			int state_len = strlen(serv_state); 	//Determine input length
			if(DEBUG) printf("[NapsterClient] %s\n",serv_state);

			//	Send the string to the server
			ssize_t numBytes = send(sock, serv_state, state_len,0);

			if (numBytes < 0) {
				DieWithSystemMessage("send() failed");
			}
			else if (numBytes != state_len) {
				DieWithUserMessage("send()", "sent unexpected number of bytes");
			}

			//receive size of list from server

			char list_size[BUFSIZE];
			if(DEBUG) printf("[NapsterClient] receiving file list size\n");
			ssize_t numBytesListSize = recv(sock,list_size,BUFSIZE,0);
			if(DEBUG) printf("[NapsterClient] Received size: %c\n",list_size[0]);

			if (numBytesListSize < 0)
				DieWithSystemMessage("recv() failed");

			//send ack to the server for list size
			char list_ack[1] = "2";
			if(DEBUG) printf("[NapsterClient] sending ACK to server\n");
			int ack_len = strlen(list_ack);
			ssize_t numBytesAck = send(sock,list_ack,ack_len,0);
			if(DEBUG) printf("[NapsterClient] sent ack to server\n");
			if(numBytesAck <0)
				DieWithSystemMessage("send() failed");

			//receive list from server
			char list_list[atoi(list_size)/sizeof(char)];
			if(DEBUG) printf("[NapsterClient] receiving file from server\n");
			ssize_t numBytesList = recv(sock,list_list,BUFSIZE,0);
			if(DEBUG) printf("[NapsterClient] Received list %s\n",list_list);

			if(numBytesList<0)
				DieWithSystemMessage("recv() failed");

			//			for()
			state = PROMPT_STATE;
		}
	}
	close(sock);
	exit(0);

}
