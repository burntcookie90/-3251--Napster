/*
 * Napster.c
 *
 *  Created on: Sep 28, 2012
 *      Author: vishnu
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"
#include "Napster.h"

#define DEBUG 1
int addFile(char* in_filename, char* client_ip){
	if(DEBUG) printf("[Napster.c] Loaded filename %s into array\n",list[file_line].filename);
	list[file_line].filename = in_filename;	//copy the bufr into the struct
	list[file_line].origin_ip_address = client_ip; //copy the ip address into the struct
	if(DEBUG) printf("[Napster.c] entering add file with %s %s\n",in_filename,client_ip);
	FILE *fp;
	if((fp = fopen(filename,"a+"))){
		fprintf(fp,"%s %s\n",list[file_line].origin_ip_address,list[file_line].filename);
		if(DEBUG) printf("[Napster.c] %s %s\n",list[file_line].origin_ip_address,list[file_line].filename);
		file_line++;
		fclose(fp);
	}
	else if(DEBUG) printf("[Napster.c] Unable to open file!\n");

	return 1;
}
