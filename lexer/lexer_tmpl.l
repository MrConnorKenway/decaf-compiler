%{
#include "build/include/parser_handwritten.tab.h"

int yycolumn = 1;
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn;	yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng;

yyFlexLexer lexer;

int yylex() {
	return lexer.yylex();
}
%}

%option yylineno
%option noyywrap

DIGIT [0-9]
ID [a-z_][A-Za-z0-9_]*
TYPE_ID [A-Z][A-Za-z0-9_]*

%x BLK_COMMENT

%%

{DIGIT}+ {
	yylval.Int_const_node_ptr_t = new Int_const_node(std::stoi(yytext));
	return c_int;
}

[-+]?{DIGIT}+"."{DIGIT}*([eE][-+]?{DIGIT}+)? {
	yylval.Double_const_node_ptr_t = new Double_const_node(std::stod(yytext));
	return c_double;
}

$rule_list

{ID} {
	yylval.Ident_node_ptr_t = new Ident_node(yytext);
	return t_ident;
}

{TYPE_ID} {
	yylval.Type_ident_node_ptr_t = new Type_ident_node(yytext);
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