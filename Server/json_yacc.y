%{
void yyerror(char* s);
int yylex();

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_util.h"
#include "server.h"

%}
%union {
	char* str;
	long integer;
	double decimal;
	struct objs* obj;
}
%start object
%token <str> STR
%token <decimal> FLOAT
%token <integer> INT TRUE FALSE
%token COLON COMMA
%token OBJ_OPEN OBJ_CLOSE
%type <obj> sub_object
%%

object:
	OBJ_OPEN series OBJ_CLOSE { return JSONSUCCESS; }
	| OBJ_OPEN OBJ_CLOSE { return JSONSUCCESS;}
	;
sub_object:
	OBJ_OPEN series OBJ_CLOSE { $$ = peek(); endObj(); }
	| OBJ_OPEN OBJ_CLOSE { $$ = peek(); endObj(); }
	;
series:
	series COMMA property_value { ; }
	| property_value { ; }
	;
property_value:
	STR COLON STR {
	 if( insertProval($1, (value_uni)$3, STR_ENM) == JSONERR ) { return JSONERR; } 
	}
	| STR COLON FLOAT {
	 if( insertProval($1, (value_uni)$3, DECIMAL_ENM) == JSONERR ) { return JSONERR; } 
	}
	| STR COLON INT {
	 if( insertProval($1, (value_uni)$3, INT_ENM) == JSONERR ) { return JSONERR; } 
	}
	| STR COLON TRUE {
	 if( insertProval($1, (value_uni)$3, INT_ENM) == JSONERR ) { return JSONERR; } 
	}
	| STR COLON FALSE {
	 if( insertProval($1, (value_uni)$3, INT_ENM) == JSONERR ) { return JSONERR; } 
	}
	| STR COLON sub_object {
	 if( insertProval($1, (value_uni)$3, OBJ_ENM) == JSONERR ) { return JSONERR; } 
	}
	;
%%

void yyerror(char* s) {
	fprintf(stderr, "%s\n", s); 
}
