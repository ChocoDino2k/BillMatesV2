#ifndef MYSERVER_H


#define MAXQUEUE 10
#define PORT 1025
//I'm not a huge fan of this
typedef enum method_enums {
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
int charToJump(char*, char);
void freeObj(obj_stc*);

#endif
