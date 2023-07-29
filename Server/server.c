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
		unsigned int bufferSize = 2048;
		char * buffer = (char *)malloc( bufferSize );
		int numBytesRead = 0;
		int totalBytesRead = 0;
		do {
			numBytesRead = read( requestSocket, buffer, bufferSize / 2);
			totalBytesRead += numBytesRead;
			printf("%d\n", numBytesRead);
			if ( totalBytesRead == sizeof(buffer) ) {
				buffer = realloc( buffer, sizeof(buffer) * 2 );
			}
			printf("%s\n", buffer);
			printf("%d %d %d %d\n", buffer[totalBytesRead - 4], buffer[totalBytesRead - 3],
			buffer[totalBytesRead - 2], buffer[totalBytesRead - 1] );
		} while(
			buffer[totalBytesRead - 4] != '\r' && buffer[totalBytesRead - 3] != '\n' &&
			buffer[totalBytesRead - 2] != '\r' && buffer[totalBytesRead - 1] != '\n'
		);
		break;
	}

	
	return 0;
}
