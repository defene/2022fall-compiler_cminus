%option noyywrap
%{
/*****************声明和选项设置  begin*****************/
#include <stdio.h>
#include <stdlib.h>

#include "syntax_tree.h"
#include "syntax_analyzer.h"

int lines = 1;
int pos_start = 0;
int pos_end = 0;

void pass_node(char *text){
     yylval.treenode = new_syntax_tree_node(text);
}

/*****************声明和选项设置  end*****************/

%}


%%
 /* to do for students */
 /* two cases for you, pass_node will send flex's token to bison */

\+   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return ADD;}
\-   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return MIN;}
"*"  {pos_start = pos_end; pos_end += 1; pass_node(yytext); return MUL;}
\/   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return DIV;}
\<   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return SMALL;}
\>   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return BIG;}
\(   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return LSMALLBRACKET;}
\)   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return RSMALLBRACKET;}
\[   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return LMIDBRACKET;}
\]   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return RMIDBRACKET;}
\{   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return LBIGBRACKET;}
\}   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return RBIGBRACKET;}
;    {pos_start = pos_end; pos_end += 1; pass_node(yytext); return SEMICOLON;}
,    {pos_start = pos_end; pos_end += 1; pass_node(yytext); return COMMA;}
=    {pos_start = pos_end; pos_end += 1; pass_node(yytext); return ASSIGN;}
\n   {pos_start = 0; pos_end = 0; lines += 1;}
" "  {pos_start = pos_end; pos_end += 1;}
\t   {pos_start = pos_end; pos_end += strlen(yytext);}
\r   {}

\<=  {pos_start = pos_end; pos_end += 2; pass_node(yytext); return SMALLEQUAL;}
\>=  {pos_start = pos_end; pos_end += 2; pass_node(yytext); return BIGEQUAL;}
==   {pos_start = pos_end; pos_end += 2; pass_node(yytext); return EQUAL;}
!=   {pos_start = pos_end; pos_end += 2; pass_node(yytext); return NOTEQUAL;}

int       {pos_start = pos_end; pos_end += 3; pass_node(yytext); return INT;}
float     {pos_start = pos_end; pos_end += 5; pass_node(yytext); return FLOAT;}
void      {pos_start = pos_end; pos_end += 4; pass_node(yytext); return VOID;}
if        {pos_start = pos_end; pos_end += 2; pass_node(yytext); return IF;}
else      {pos_start = pos_end; pos_end += 4; pass_node(yytext); return ELSE;}
while     {pos_start = pos_end; pos_end += 5; pass_node(yytext); return WHILE;}
return    {pos_start = pos_end; pos_end += 6; pass_node(yytext); return RETURN;}

[a-zA-Z]+                {pos_start = pos_end; pos_end += strlen(yytext); pass_node(yytext); return ID;}
[0-9]+                   {pos_start = pos_end; pos_end += strlen(yytext); pass_node(yytext); return INTEGER;}
[0-9]*\.[0-9]+|[0-9]+\.  {pos_start = pos_end; pos_end += strlen(yytext); pass_node(yytext); return FLOATPOINT;}

\/[*][^*]*[*]+([^/*][^*]*[*]+)*\/ {pos_start=0; pos_end=0; for(int i=0;i<strlen(yytext);i++) {if(yytext[i] == '\n') lines++; }}

.    {pos_start = pos_end; pos_end += 1; return ERROR;}
%%
