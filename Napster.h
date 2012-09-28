/*
 * Napster.h
 *
 *  Created on: Sep 28, 2012
 *      Author: vishnu
 */

#ifndef NAPSTER_H_
#define NAPSTER_H_

//Function to add file to the napster server database
int addFile(char* filename, char* client_ip);

//Function to delete file from the napster server database
int deleteFile(char* filename, char* client_ip);

//Function to list files from the napster server database
void getFileList();

#endif /* NAPSTER_H_ */
