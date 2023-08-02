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

//I'm not a huge fan of this
typedef enum {
	UNKOWN,
	LOGIN,
	REGISTER
} method_enm;

typedef struct {
	char* username;
	char* password;
} login_stc;

typedef struct {
	char* name;
	char* password;
} register_stc;


void parseLogin(login_stc*, char*);
int createServerSocket();
void setupInterruptHandlers();
void sigInterrupt(int);
int charToJump( char*, char);
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
	//TODO: FREE MEMORY
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
		struct sockaddr_in clientIPAddr;
		int alen = sizeof( clientIPAddr );
		int requestSocket = accept( server, 
				(struct sockaddr *)&clientIPAddr,
				(socklen_t *)&alen );
		if ( requestSocket < 0 ) {
			perror("accept");
			exit(1);
		}
		char clientIP[INET_ADDRSTRLEN];
		inet_ntop( AF_INET, &clientIPAddr, clientIP, INET_ADDRSTRLEN );
		printf("ACCEPTED CONNECTION FROM: %s\n", clientIP );
		//Process the request
		unsigned int bufferSize = 1024; //no significant reason for 1024
		char * buffer = (char *)malloc( bufferSize );
		int numBytesRead = 0;
		int totalBytesRead = 0;
		char readAllHeaders = 0;
		char readAnotherRequest = 0;
		unsigned int headersSize = 0;
		unsigned int bodySize = 0;
		int num = 0;
		do {
			//read an entire request
			memset(buffer, 0, bufferSize);
			totalBytesRead = 0;
			readAllHeaders = 0;
			readAnotherRequest = 0;
			bodySize = 0;
			headersSize = 0;
			do {
				numBytesRead = read( requestSocket, buffer + totalBytesRead, bufferSize / 2);
				totalBytesRead += numBytesRead;
				if ( totalBytesRead == bufferSize ) {
					bufferSize *= 2;
					buffer = realloc( buffer, bufferSize);
				}
				//check if read all headers
				int i = totalBytesRead; 
				while ( !readAllHeaders & (i > 3) ) {
					if (
							buffer[i - 4] == '\r' && buffer[i - 3] == '\n' &&
							buffer[i - 2] == '\r' && buffer[i - 1] == '\n'
						 ) {
						readAllHeaders = 1;
						headersSize = i - 4;
						//get the size of the body of the request -> 0 means GET request
						int j = 0;
						char contentLength[100];
						while ( j < headersSize ) {
							if ( strncmp( (buffer + j), "Content-Length:", 15 ) == 0 ) {
								j += 16;
								int numDigits = charToJump( buffer + j, '\r' ) - 1;
								strncpy(contentLength, buffer + j, numDigits);
								contentLength[numDigits] = '\0';
								bodySize = atoi(contentLength);
								j += numDigits + 2;
							} else if ( strncmp( (buffer + j), "Connection: keep-alive", 22) == 0 ) {
								readAnotherRequest = 1;
								j += 24;
							} else {
								j += charToJump(buffer + j, '\n');
							}
						}
					}
					i--;
				}

				printf("size of headers: %d\n", headersSize + 4);
				printf("size of body: %d\n", bodySize);
				printf("total bytes read: %d\n", totalBytesRead);

			} while( numBytesRead > 0 && (headersSize + bodySize + 4) < totalBytesRead );
			printf("%s\n", buffer);
			printf("SENDING RESPONSE\n");
			char * response = 
			"HTTP/1.1 200 OK\r\n"
			"Access-Control-Allow-Origin: *\r\n"
			"Access-Control-Allow-Methods: GET,HEAD,OPTIONS,POST\r\n"
			"Access-Control-Allow-Headers: Access-Control-Allow-Headers,Origin,Accept,X-Requested-With,Content-Type,Access-Control-Request-Method,Access-Control-Request-Headers\r\n"
			"Content-Length: 5\r\n"
			"Connection: close\r\n"
			"Keep-Alive: timeout=1000, max=10\r\n"
			"\r\n"
			"hello";
			if ( strncmp(buffer, "OPTIONS", 7) == 0 ) {
			response = "HTTP/1.1 200 OK\r\n"
			"Access-Control-Allow-Origin: *\r\n"
			"Access-Control-Allow-Methods: GET,HEAD,OPTIONS,POST\r\n"
			"Access-Control-Allow-Headers: Access-Control-Allow-Headers,Origin,Accept,X-Requested-With,Content-Type,Access-Control-Request-Method,Access-Control-Request-Headers\r\n"
			"Connection: keep-alive\r\n"
			"Content-Length: 5\r\n"
			"Keep-Alive: timeout=10000, max=10\r\n"
			"\r\n"
			"hello";
			}
			if ( strncmp(buffer, "POST", 4) == 0 ) {
				login_stc login;
				parseLogin(&login, buffer + headersSize + 4);
				printf("username parsed: %s\n", login.username);
				printf("password parsed: %s\n", login.password);
			}
			num = send( requestSocket, response, strlen(response), 0);
			printf("DONE SENDING BYTES: %d/%ld\n", num, strlen(response));

		} while( readAnotherRequest );

		//Free memory and close down the processes
		free( buffer );
		buffer = NULL;
		printf("Closing connection\n");
		shutdown( requestSocket, SHUT_RDWR );
		close( requestSocket );
	}


	return 0;
}


int charToJump(char * string, char character) {
	unsigned int i = 0;
	while ( *(string + i) != character) {	
		i++;
	}
	return i + 1;
}

void parseLogin(login_stc* login, char* body) {
	login->username = "my username";
	login->password = "my password";
}
