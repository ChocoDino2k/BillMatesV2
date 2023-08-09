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

char* encodeJson(obj_stc* root) {
	unsigned int posStack[MAXOBJDEPTH];
	posStack[0] = -1;
	unsigned int posStackSpot = 1;
	obj_stc* ptrStack[MAXOBJDEPTH];
	ptrStack[0] = root;
	unsigned int ptrStackSpot = 1;
	
	int resultSize = 512;
	char* result = (char*)malloc(resultSize);
	result[0] = '{';
	int propLen = 0;
	int valueLen = 0;
	int currentSize = 1;
	obj_stc* currentPtr;
	int i;

	while( ptrStackSpot != 0 ) {
		currentPtr = ptrStack[ptrStackSpot - 1];
		i = posStack[posStackSpot - 1] + 1;
		while ( i < currentPtr->length ) {
			propLen = strlen( currentPtr->items[i].property ) + 2;
			printf("looking at property: '%s' at position %d\n", currentPtr->items[i].property, i);
			switch (currentPtr->items[i].type) {
				case STR_ENM:
					valueLen = strlen( currentPtr->items[i].value.str_value ) + 2;
					while( result != NULL && propLen + valueLen + currentSize + 2 > resultSize ) { 
						result = resize(result, &resultSize);
					}
					if ( result == NULL) { //idk what to do tbh
						return NULL;
					}
					sprintf(result + currentSize, "\"%s\":\"%s\",",
									currentPtr->items[i].property, currentPtr->items[i].value.str_value);
					currentSize += propLen + valueLen + 2;
					break;
				case DECIMAL_ENM:
					valueLen = 25;
					while( result != NULL && propLen + valueLen + currentSize + 2 > resultSize ) { 
						result = resize(result, &resultSize);
					}
					if ( result == NULL) { //idk what to do tbh
						return NULL;
					}					
					sprintf(result + currentSize, "\"%s\":%f,", 
									currentPtr->items[i].property, currentPtr->items[i].value.decimal_value);
					currentSize += propLen + valueLen + 2;
					break;
				case INT_ENM:
					valueLen = 25;
					while( result != NULL && propLen + valueLen + currentSize + 2 > resultSize ) { 
						result = resize(result, &resultSize);
					}
					if ( result == NULL) { //idk what to do tbh
						return NULL;
					}					
					sprintf(result + currentSize, "\"%s\":%ld,", 
									currentPtr->items[i].property, currentPtr->items[i].value.int_value);
					currentSize += propLen + valueLen + 2;
					break;
				case OBJ_ENM:
					valueLen = 1;
					while ( result != NULL && currentSize + propLen + valueLen + 1 > resultSize ) {
						result = resize(result, &resultSize);
					}
					if ( result == NULL ) {
						return NULL;
					}
					sprintf(result + currentSize, "\"%s\":{", currentPtr->items[i].property);
					currentSize += propLen + valueLen + 1;
					currentPtr = currentPtr->items[i].value.obj_value;
					posStack[posStackSpot - 1] = i;
					i = -1;
					ptrStack[ptrStackSpot] = currentPtr;
					ptrStackSpot++;
					posStack[posStackSpot] = 0;
					posStackSpot++;
					break;
				default:
					break;
			} //end switch
			i++;
		} //end for
		if ( currentPtr->length == 0 ) { //edge case for empty object
			currentSize++;
		}
		while ( result != NULL && currentSize + 1 > resultSize ) {
			result = resize(result, &resultSize);
		}
		if ( result == NULL ) {
			return NULL;
		}
		result[currentSize - 1] = '}';
		result[currentSize] = ',';
		currentSize++;
		ptrStackSpot--;
		posStackSpot--;
	}
	result[currentSize - 1] = '\0';
	return result;
}

char* resize(char* old, int* size) {
	printf("new size: %d\n", *size * 2);
	char* resized = (char*)realloc(old, *size * 2);
	*size *= 2;
	return resized;
}

