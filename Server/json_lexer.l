%{
#include "json_util.h"
#include "server.h"
#include "y.tab.h"

void yyerror(char *s);
int yylex();
%}

%option nounput
%option noinput
%%

[\"]([ !#-\[\]-~]|\\[\"\\bfnrt\/])*[\"] {
	//strip away quotation marks
	int len = strlen(yytext);
	yylval.str = strndup(yytext + 1, len - 2);
	return STR;
}

[-]?[0-9]+[\.]{1}[0-9]+ {
	yylval.decimal = atof(yytext);
	return FLOAT;
}

[-]?[0-9]+ {
	yylval.integer = atoi(yytext);
	return INT;
}

true {
	yylval.integer = 1;
	return TRUE;
}

false {
	yylval.integer = 0;
	return FALSE;
}

\{ {
	startObj();
	return OBJ_OPEN;
}

\} {
	return OBJ_CLOSE;
}

: {
	return COLON;
}

, {
	return COMMA;
}

[ \t\n] ;

. {
	ECHO;
	yyerror("unexpected character");
}

%%

int yywrap(void) { return 1; }
