%{ /* -*- C++ -*- */
# include <cstdlib>
# include <cerrno>
# include <climits>
# include <string>
# include <iostream>

# include "Parser.h"
# include "y.tab.h"

/* Work around an incompatibility in flex (at least versions
	2.5.31 through 2.5.33): it generates code that does
	not conform to C89.  See Debian bug 333231
	<http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.  */
# undef yywrap
# define yywrap() 1

int lineno = 1;

/* By default yylex returns int, we use token_type.
	Unfortunately yyterminate by default returns 0, which is
	not of token_type.  */
#define yyterminate() return token::END
%}

%option noyywrap nounput debug

%{
# define YY_USER_ACTION  yylloc->columns (yyleng);
%}

%%

%{  
	yylloc->step ();	
%}  
  
%{  
    typedef yy::SqlParser::token token;  
%}

select	{ return token::SELECT; }
insert	{ return token::INSERT; }
delete	{ return token::DELETE; }
create	{ return token::CREATE; }
drop	{ return token::DROP; }
table	{ return token::TABLE; }
index	{ return token::INDEX; }
values	{ return token::VALUES; }
null	{ return token::NULLX; }
COMPARISON { return token::COMPARISON; }
from	{ return token::FROM; }
where	{ return token::WHERE; }
or		{ return token::OR; }
and		{ return token::AND; }
not 	{ return token::NOT; }
primary	{ return token::PRIMARY; }
key 	{ return token::KEY; }
all		{ return token::ALL; }
distinct	{return token::DISTINCT; }
on		{ return token::ON; }
unique	{ return token::UNIQUE; }
into	{ return token::INTO; }
int		{ return token::INT; }
char	{ return token::CHAR; }
float	{ return token::FLOAT; }
exit	{ return token::EXIT; }
execfile { return token::EXEC; }
show	{ return token::SHOW; }
desc	{ return token::DESC; }
	/* punctuation */

"="		|
"<>" 	|
"<"		|
">"		|
"<="	|
">="		{ 
	yylval->strval = new std::string (yytext);
	return token::COMPARISON; 
}

[-+*/:(),.;]	{
	return yy::SqlParser::token_type(yytext[0]); 
};

	/* names */

[A-Za-z][A-Za-z0-9_.]*	{
	yylval->strval = new std::string (yytext);	
	return token::NAME; 
};

	/* numbers */
[0-9]+	{	
	long n = strtol (yytext, NULL, 10);  
	if (! (INT_MIN <= n && n <= INT_MAX && errno != ERANGE))  
		driver.error (*yylloc, "integer is out of range");
	yylval->strval = new std::string (yytext);
	return token::INTNUM; 
};

[0-9]+"."[0-9]* |
"."[0-9]*	{	
	double n = strtod (yytext, NULL);	
	if (errno == ERANGE)  
		driver.error (*yylloc, "float is out of range");
	yylval->strval = new std::string (yytext);	
	return token::APPROXNUM; 
};

	/* strings */

'[^'\n]*'	{		
	yylval->strval = new std::string (yytext);    
	return token::STRING;	
};
		
'[^'\n]*$	{	
	driver.error("Unterminated string"); 
};

\n	{
	lineno++; yylloc->lines (yyleng); yylloc->step ();	
	driver.printHead();
};

[ \t\r]+ {	
};	/* white space */

"--".*$	{
};	/* comment */


%%

int
Parser::scan_begin ()
{
	yy_flex_debug = trace_scanning;	
	if (file.empty () || file == "-") {
		yyin = stdin;		
	yyrestart(yyin);
	} else if (!(yyin = fopen (file.c_str (), "r")))
	{
		error ("cannot open " + file);
		return Parser::SYNTAX_ERR;
	}
}

void
Parser::scan_end ()
{
	if (file.empty () || file == "-") {
	
	} else {
		fclose (yyin);
	}	
}
