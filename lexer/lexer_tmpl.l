%{
#include <math.h>
#include <assert.h>
#include "parser_gen.tab.h"

int yycolumn = 1;
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn;	yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng;
%}

%option yylineno

DIGIT [0-9]
ID [a-z_][A-Za-z0-9_]*
TYPE_ID [A-Z][A-Za-z0-9_]*

%x BLK_COMMENT

%%

{DIGIT}+ {
	return c_int;
}

[-+]?{DIGIT}+"."{DIGIT}*([eE][-+]?{DIGIT}+)? {
	return c_double;
}

$rule_list

{ID} {
	return t_ident;
}

{TYPE_ID} {
	return t_type_ident;
}

[ \t]+ // skip whitespace

[\n\r]+ {
	// skip newline 
	yycolumn = 1;	
}

"//".*\n

"/*" {
	BEGIN(BLK_COMMENT);
}
<BLK_COMMENT>"*/" {
	BEGIN(INITIAL);
}
<BLK_COMMENT>\n { 
	yycolumn = 1; 
}
<BLK_COMMENT>. { }

<<EOF>> {
	return 0;
}

. {
	printf("Unknown token\n");
}

%%

int main(int argc, char **argv) {
	++argv, --argc;  /* skip over program name */
	if (argc > 0) {
		assert(argc == 1);
		yyin = fopen(argv[0], "r");
	}
	else {
		yyin = stdin;
	}

	yyparse();

	return 0;
}