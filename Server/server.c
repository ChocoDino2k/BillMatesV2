#include <assert.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>


int createServerSocket();
void setupInterruptHandlers();
void sigInterrupt(int);
int charUntil( char*, char);
#define MAXQUEUE 10
#define PORT 1025

void setupInterruptHandlers() {
	struct sigaction signalAction;
	signalAction.sa_handler = sigInterrupt;
	sigemptyset( &signalAction.sa_mask);
	signalAction.sa_flags = SA_RESTART;

	//redirect interrupts to sigInterrupt funtion
	assert( sigaction( SIGINT, &signalAction, NULL) == 0 );
	assert( sigaction( SIGPIPE, &signalAction, NULL) == 0);
	assert( sigaction( SIGCHLD, &signalAction, NULL) == 0);
	assert( sigaction( SIGINT, &signalAction, NULL) == 0);
}

void sigInterrupt( int sigNum ) {
	switch ( sigNum) {
		case SIGINT:
			//close( serverSock);
			while( waitpid(-1, NULL, WNOHANG) > 0);
			printf("Exiting from ^C\n");
			exit(1);
			break;
		case SIGCHLD:
			printf("Received a SIGCHLD\n");
		case SIGPIPE:
			while (waitpid(-1, NULL, WNOHANG) > 0);
			printf("Received a SIGPIPE\n");
			break;
		default:
			printf("Received unknown error: %d\n", sigNum);
	}
}

//initializes and allocates a socket that is to be used as the server
int createServerSocket() {
	int port = PORT;

	// Set the IPv4 addr and port for the server
	struct sockaddr_in serverIPAddr;
	memset( &serverIPAddr, 0, sizeof(serverIPAddr) );
	serverIPAddr.sin_family = AF_INET;
	serverIPAddr.sin_addr.s_addr = INADDR_ANY;
	serverIPAddr.sin_port = htons( (u_short) port);

	//Allocate a socket to represent the server
	int serverSock = socket( PF_INET, SOCK_STREAM, 0);
	assert( serverSock >= 0);

	//interrupt handlers
	setupInterruptHandlers();

	//establishing socket options
	int optval = 1;
	assert( setsockopt( serverSock, SOL_SOCKET, SO_REUSEADDR,
				(char *) &optval, sizeof( optval ) ) == 0);

	//Binding the socket
	assert( bind( serverSock, (struct sockaddr *) &serverIPAddr,
				sizeof( serverIPAddr) ) == 0);

	//display the hostname:port of the server
	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023); 
	//converting to a IPv4 address
	printf("IP Address -> %s:%d\n", inet_ntoa(serverIPAddr.sin_addr), port);
	printf("URL -> %s:%d\n", hostname, port);

	//set server to begin listening
	assert( listen( serverSock, MAXQUEUE) == 0 );

	return serverSock;
}

int main() {
	printf("hello world\n");

	int server = createServerSocket();
	while ( 1 ) {
		struct sockaddr_in clientIPAddress;
		int alen = sizeof( clientIPAddress );
		int requestSocket = accept( server, 
			(struct sockaddr *)&clientIPAddress,
			(socklen_t *)&alen );
		if ( requestSocket < 0 ) {
			perror("accept");
			exit(1);
		}

		//Process the request
		unsigned int bufferSize = 64;
		char * buffer = (char *)malloc( bufferSize );
		int numBytesRead = 0;
		int totalBytesRead = 0;
		char quit = 0;
		char readAllHeaders = 0;
		unsigned int headersSize = 0;
		//unsigned int bodySize = 0;
		while (!quit) {
			numBytesRead = read( requestSocket, buffer + totalBytesRead, bufferSize / 2);
			totalBytesRead += numBytesRead;
			//printf("%d\n", numBytesRead);
			if ( totalBytesRead == bufferSize ) {
				bufferSize *= 2;
				buffer = realloc( buffer, bufferSize);
			}
			//printf("%s\n", buffer);
			//printf("%d %d %d %d\n", buffer[totalBytesRead - 4], buffer[totalBytesRead - 3],
			//buffer[totalBytesRead - 2], buffer[totalBytesRead - 1] );

			//check if read all headers
			int i = totalBytesRead; 
			//printf("total num of bytes: %d\n", i);
			//printf("%s\n", buffer);
			while ( !readAllHeaders & (i > 3) ) {
				if (
				buffer[i - 4] == '\r' && buffer[i - 3] == '\n' &&
				buffer[i - 2] == '\r' && buffer[i - 1] == '\n'
				) {
					readAllHeaders = 1;
					headersSize = i;
				}
				if (buffer[i] == '\r') {
					printf("carriage return\n");
				} else if (buffer[i] == '\n') {
					printf("line feed\n");
				} else {
					printf("%c", buffer[i]);
				}
				i--;
			}

			if (numBytesRead == 0) {
				quit = 1;
			} else if ( readAllHeaders ) {
				char contentLength[200]; //way larger than need be
				//char sawCarriageReturn = 0;
				int j = 0;
				while ( j < headersSize ) {
					//strncpy(contentLength, buffer, 15);
					if ( strncmp(buffer + j, "Content-Length:", 15) ) {
						j += 15;
						strncpy(contentLength, buffer + j, charUntil(buffer + j, '\r') );
						contentLength[ charUntil(buffer + j, '\r') ] = '\0';
						printf("%s\n", contentLength);
					} else {
						j += charUntil(buffer + j, '\n');
					}
				}
			}
		}
		printf("finished\n");
		printf("size of headers: %d\n", headersSize);
	}

	
	return 0;
}


int charUntil(char * string, char character) {
	unsigned int i = 0;
	while ( *(string + i) != character) {
		printf("%c", *(string + i) );
		i++;
	}
	return i;
}
