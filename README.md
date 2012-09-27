Compiling!

type:
  $ make

Running the Server:
	./NapsterServer <port>

Running the client
	./NapsterClient <server-ip> <port>


Protocol:
	Client has 3 functions: add, delete and listing of files.
	Add: Send filename with a 1 prepending the file name, this tells the server which mode to switch to.
	Delete: Send filename with a 3 prepending the file name, this tells the server which mode to switch to.
	List: Send a 2 to the server. Server sends the size of the file list and waits for an ack from the client. Server then sends the file list.


Bugs:
	Server state does not set correctly after sending filelist, cannot execute other commands.
	Delete is not implemeneted.
