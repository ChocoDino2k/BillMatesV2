#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json_util.h"

unsigned char jsonErrno;

unsigned int currentStackSpot = 0;
obj_stc* objectStack[MAXOBJDEPTH];

void dumpJson(obj_stc* json, int indent) {
	for( int i = 0; i < json->length; i++ ) {
		for( int j = 0; j < indent; j++) { printf(" "); }
		printf("property: '%s'", json->items[i].property);
		switch (json->items[i].type) {
			case STR_ENM:
				printf(" value: '%s'\n", json->items[i].value.str_value);
				break;
			case DECIMAL_ENM:
				printf(" value: %f\n", json->items[i].value.decimal_value);
				break;
			case INT_ENM:
				printf(" value: %ld\n", json->items[i].value.int_value);
				break;
			case OBJ_ENM:
				printf(" value: object\n");
				dumpJson( json->items[i].value.obj_value, indent + 2 );
				break;
			default:
				printf(" value: unknown?\n");
		}
	}
}


int insertProval( char* prop, value_uni value, proval_enm type) {
	obj_stc* currentObj = peek();
	if ( currentObj->length == MAXPROPERTIES) { jsonErrno = MAXPROPERTIESERR; return JSONERR; } 
	currentObj->items[currentObj->length].property = prop;
	currentObj->items[currentObj->length].value = value;
	currentObj->items[currentObj->length].type = type;
	currentObj->length++;
	return JSONSUCCESS;
}

int push( obj_stc* ptr ) {
	if ( currentStackSpot == MAXOBJDEPTH ) { jsonErrno = MAXOBJDEPTHERR; return JSONERR; }
	objectStack[currentStackSpot] = ptr;
	currentStackSpot += 1;
	return JSONSUCCESS;
}

obj_stc* pop() {
	if ( currentStackSpot == 0 ) { return NULL; }
	currentStackSpot -= 1;
	return objectStack[currentStackSpot];
}

obj_stc* peek() {
	return objectStack[currentStackSpot - 1];
}

int startObj() {
	obj_stc* obj = (obj_stc*) malloc( sizeof( obj_stc ) );
	obj->length = 0;
	return push(obj);
}

int endObj() {
	return (pop() == NULL);
}

void freeObj(obj_stc* obj) {
	if ( obj == NULL ) { return; }
	for ( int i = 0; i < obj->length; i++) {
		free(obj->items[i].property);
		obj->items[i].property = NULL;
		if( obj->items[i].type == STR_ENM ) {
			free( obj->items[i].value.str_value );
			obj->items[i].value.str_value = NULL;
		} else if ( obj->items[i].type == OBJ_ENM ) {
			freeObj( obj->items[i].value.obj_value );
		}
	}
	obj->length = 0;
	free(obj);
}

int getPropertyIdx(obj_stc* obj, const char* property) {
	if ( obj == NULL ) { return -1; }
	int propLen = strlen(property);
	int i = 0;
	while( 
				( i < obj->length ) && 
				( strlen(obj->items[i].property) == propLen ) &&
				( strncmp(property, obj->items[i].property, propLen) ) 
			 ) 
	{
		i++;
	}
	return ((i + 1) % (obj->length + 1)) - 1; //returns -1 if i == obj->length
}

