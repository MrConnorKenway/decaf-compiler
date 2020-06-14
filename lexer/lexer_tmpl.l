%{
#include "build/parser.tab.h"

void yyerror(const char *s);
int yycolumn = 1;
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn;	yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng;

static std::string strbuf;
%}

%option yylineno
%option noyywrap
%option debug
%option c++

DIGIT [0-9]
ID [a-z_][A-Za-z0-9_]*
TYPE_ID [A-Z][A-Za-z0-9_]*

%x BLK_COMMENT
%x STR_LITERAL

%%

{DIGIT}+ {
	yylval.Int_const_node_ptr_t = new Int_const_node(std::stoi(yytext));
	yylval.Int_const_node_ptr_t->yylloc_ptr = new YYLTYPE(yylloc);
	return c_int;
}

[-+]?{DIGIT}+"."{DIGIT}*([eE][-+]?{DIGIT}+)? {
	yylval.Double_const_node_ptr_t = new Double_const_node(std::stod(yytext));
	yylval.Double_const_node_ptr_t->yylloc_ptr = new YYLTYPE(yylloc);
	return c_double;
}

\" {
    BEGIN(STR_LITERAL);
}
<STR_LITERAL>[^\\"\n]* {
    strbuf += yytext;
}
<STR_LITERAL>\\n {
    strbuf += '\n';
}
<STR_LITERAL>\\t {
    strbuf += '\t';
}
<STR_LITERAL>\\[\\"] {
    strbuf += yytext[1];
}
<STR_LITERAL>\n {
    yyerror("New line in string");
}
<STR_LITERAL>\\. {
    yyerror("Unknown escape sequence");
}
<STR_LITERAL>\" {
    yylval.Str_const_node_ptr_t = new Str_const_node(strbuf);
    yylval.Str_const_node_ptr_t->yylloc_ptr = new YYLTYPE(yylloc);
    BEGIN(INITIAL);
    return c_string;
}

$rule_list

{ID} {
	yylval.Ident_node_ptr_t = new Ident_node(yytext);
	yylval.Ident_node_ptr_t->yylloc_ptr = new YYLTYPE(yylloc);
	return t_ident;
}

{TYPE_ID} {
	yylval.User_defined_type_node_ptr_t = new User_defined_type_node(yytext);
	yylval.User_defined_type_node_ptr_t->yylloc_ptr = new YYLTYPE(yylloc);
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