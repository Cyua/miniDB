#ifndef _PARSER_H
#define _PARSER_H

#include <iostream>
#include <string>
#include <vector>

#include "Expression.h"
#include "Condition.h"
#include "Schema.h"
#include "Table.h"
#include "y.tab.h"

// Tell Flex the lexer's prototype ...
# define YY_DECL										\
	yy::SqlParser::token_type						\
	yylex (yy::SqlParser::semantic_type* yylval,	\
	yy::SqlParser::location_type* yylloc,			\
	Parser& driver)

// ... and declare it for the parser's sake.
YY_DECL;


class Parser{
private:
	int line;
	bool echoEn;
	bool newSmt;
	bool err;	
	Condition* condition;	
public:
	std::string sqlFileName;
	Parser();
	virtual ~Parser();
	
	Schema* schema;
	Table* table;
	std::vector<std::pair<std::string, std::string> >* values;
	std::vector<std::pair<std::string, std::string> >* cs;
	int result;

	// Handling the scanner.
	int scan_begin ();
	void scan_end ();
	bool trace_scanning;
	
	void setEcho(bool echo);
	bool getEcho();
	
	void setNewSmt(bool smt);
	bool getNewSmt();
	
	void printHead();

	// Run the parser.  Return 0 on success.
	int parse (const std::string& f);
	std::string file;
	bool trace_parsing;

	// Error handling.
	void error (const yy::location& l, const std::string& m);
	void error (const std::string& m);
	
	void setErr(bool err);
	bool getErr();
	
	Condition* getCondition();
	
	const static int SYNTAX_ERR;
	const static int EXIT;
	const static int EXEC;
};

#endif