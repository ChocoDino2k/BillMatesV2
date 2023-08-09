#ifndef MYJSONUTIL_H

#define MAXPROPERTIES 32
#define MAXOBJDEPTH 8

#define JSONSUCCESS 1
#define JSONERR 0
#define MAXPROPERTIESERR 2
#define MAXOBJDEPTHERR 3
typedef enum proval_enums {
	INT_ENM,
	DECIMAL_ENM,
	STR_ENM,
	CHAR_ENM,
	OBJ_ENM
} proval_enm;

typedef union value_union {
	char* str_value;
	long int_value;
	double decimal_value;
	char char_value;
	struct objs* obj_value; //this is of type series
} value_uni;

typedef struct proval_stc {
	char* property; //could probably change this to fixed width
	value_uni value;
	proval_enm type;
} proval;

typedef struct proval_stc* series;

typedef struct objs {
	proval items[MAXPROPERTIES];
	unsigned int length;
} obj_stc;

int insertProval(char*, value_uni, proval_enm);
int push(obj_stc*);
obj_stc* pop();
obj_stc* peek();
int startObj();
int endObj();
void dumpJson(obj_stc*, int);
int getPropertyIdx(obj_stc*, const char*);
char* resize(char*, int*);
char* encodeJson(obj_stc*);
#endif
