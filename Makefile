cc = gcc
CFLAGS = -std=gnu99 -pedantic -Wall -Werror
RELEASE_FLAGS = -O2
SERVER_FILES = AddressUtility.c DieWithMessage.c NapsterServer.c NapsterServerUtility.c
CLIENT_FILES = NapsterClient.c DieWithMessage.c 

all: client server


server : $(SERVER_FILES)
	$(cc) $(CFLAGS) $(SERVER_FILES) -o NapsterServer

client : $(CLIENT_FILES)
	$(cc) $(CFLAGS) $(CLIENT_FILES) -o NapsterClient

clean : 
	rm NapsterServer NapsterClient 

#TCPServerUtility.o : TCPServerUtility.c Practical.h
	#$(cc) $(CFLAGS) TCPServerUtility.c

#DieWithMessage.o : DieWithMessage.c
	#$(cc) $(CFLAGS) DieWithMessage.c

#TCPEchoServer4.o : TCPEchoServer4.c Practical.h
	#$(cc) $(CFLAGS) TCPEchoServer4.c

#TCPEchoClient4.o : TCPEchoClient4.c Practical.h
	#$(cc) $(CFLAGS) TCPEchoClient4.c

#AddressUtility.o : AddressUtility.c
	#$(cc) $(CFLAGS) AddressUtility.c
