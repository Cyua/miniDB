%skeleton "lalr1.cc" /* -*- C++ -*- */
//%require "2.7"
%defines
%define parser_class_name "SqlParser"

%code requires {
# include <string>
# include <sstream>
# include <cstdlib>
# include <stdexcept>
class Parser;
class Schema;
class Condition;
}

// The parsing context.
%parse-param { Parser& driver }
%lex-param   { Parser& driver }

%locations
%initial-action
{
// Initialize the initial location.
@$.begin.filename = @$.end.filename = &driver.file;
};

%debug
%error-verbose

// Symbols.
%union {
	int intval;
	double floatval;
	std::string *strval;
	int subtok;
	Condition* condition;
	Schema* schema;
}

%token <strval> NAME
%token <strval> STRING
%token <strval> INTNUM 
%token <strval> APPROXNUM

%type <strval> column
%type <strval> index
%type <strval> column_def_opt
%type <strval> data_type
%type <strval> scalar_exp
%type <strval> atom
%type <strval> insert_atom
%type <strval> literal
%type <strval> column_ref
%type <strval> table
%type <intval> selection
%type <strval> table_ref
	/* operators */

%left OR
%left AND
%left NOT
%left <strval> COMPARISON /* = <> < > <= >= */
%left '+' '-'
%left '*' '/'

%token END 0 "end of file"  

%token SELECT INSERT DELETE CREATE DROP EXIT
%token TABLE INDEX VALUES NULLX COMPARISON
%token FROM WHERE OR AND NOT PRIMARY KEY
%token ALL DISTINCT ON UNIQUE INTO
%token INT CHAR FLOAT
%token EXEC
%token SHOW
%token DESC

%code {
# include "Parser.h"
# include "Schema.h"
# include "Condition.h"
# include "WickyEngine.h"
# include "Table.h"
}


%%

sql_list:
		sql ';' { driver.setNewSmt(true); }
	|	sql_list sql ';' { driver.setNewSmt(true); }
	|	EXIT { return Parser::EXIT; }
	|	sql_list EXIT { return Parser::EXIT; }
	;
	
	/* schema definition language */	
sql:
		base_table_def
	|	base_index_def
	|	drop_table
	|	drop_index
	|	show_tables
	|	show_indexs
	|	desc_table
	;

show_tables:
		SHOW TABLE{
			WickyEngine* we = WickyEngine::getInstance();
			we->ShowTables();
		}
	;
	
show_indexs:
		SHOW INDEX{
			
		}
	;

desc_table:
		DESC TABLE NAME{
				WickyEngine* we = WickyEngine::getInstance();
			we->DescribeTable(*$3);
			delete $3;
		}
	
base_table_def:
		CREATE TABLE def_table '(' base_table_element_commalist ')' {
			WickyEngine* we = WickyEngine::getInstance();
			try {
				we->CreateTable(*(driver.schema));
			} catch (std::runtime_error& e){
				driver.error(e.what());
				return Parser::SYNTAX_ERR;
			}
			delete driver.schema;
			driver.schema = NULL;
		}
	;
	
def_table:
		table {
			driver.schema = new Schema(*$1);			
		}
	;
	
base_table_element_commalist:
		base_table_element {
			
		}
	|	base_table_element_commalist ',' base_table_element {
				
		}
	;

base_table_element:
		column_def
	|	table_constraint_def
	;
	
base_index_def:
		CREATE INDEX index ON table '(' column ')' {
			WickyEngine* we = WickyEngine::getInstance();
			try {
				we->createIndex(*$3, *$5, *$7);
				delete $3;
				delete $5;
				delete $7;
			} catch (std::runtime_error& e){
				driver.error(e.what());
			}
		}
	;
		
drop_table:
		DROP TABLE table{
			WickyEngine* we = WickyEngine::getInstance();
			try {
				we->DropTable(*$3);
			} catch (std::runtime_error& e){
				driver.error(e.what());
				return Parser::SYNTAX_ERR;
			}		
			delete $3;
		}
	;

drop_index:
		DROP INDEX index ON table {
			WickyEngine* we = WickyEngine::getInstance();
			try {
				we->dropIndex(*$3, *$5);
				delete $3;				
			} catch (std::runtime_error& e){
				driver.error(e.what());
			}
		}
	;

column_def:
		column data_type column_def_opt {
			driver.schema->addAttribute(*$1, $2[0]);
			int len = std::atoi($2[1].c_str());
			driver.schema->setLength(*$1, len);			
			if ($3 != NULL){
				if (*$3 == Schema::UNIQUE)
					driver.schema->setUnique(*$1);
				else
					driver.error("WickyDB do not support constraint " + *$3);
			}
			delete $1;
			delete[] $2;
			delete $3;
		}
	;
	
column_def_opt:
		/* empty */{
			$$ = NULL;
		}
	|	UNIQUE {
			$$ = new std::string(Schema::UNIQUE);
		}
	;	
	
table_constraint_def:		
	|	PRIMARY KEY '(' column ')' {
			driver.schema->setPrimaryKey(*$4);
			delete $4;
		}
	;
	
sql:
		manipulative_statement
	|	exec_statement
	;


exec_statement:
		EXEC NAME {
			driver.sqlFileName = * $2;		
			delete $2;
			return Parser::EXEC;
		}
	;
	
manipulative_statement:
		delete_statement_searched
	|	insert_statement
	|	select_statement
	;

delete_statement_searched:
		DELETE FROM table opt_where_clause{
			WickyEngine* we = WickyEngine::getInstance();
			Table* t = NULL;
			try {								
				//t=we->GetTable(*$3);				
				//we->Delete(t, *(driver.getCondition()));
				//delete t;
				we->DeleteByName(*$3, *(driver.getCondition()));				
				delete $3;
			} catch (std::runtime_error& e){
				if (t != NULL){
					delete t;
					t = NULL;
				}		
				driver.error(e.what());
				return Parser::SYNTAX_ERR;
			}			
		}
	;
	
insert_statement:
		INSERT INTO table values_or_query_spec {
			WickyEngine* we = WickyEngine::getInstance();
			Table* t = NULL;
			try {
				//t=we->GetTable(*$3);
				we->InsertByName(*$3, *(driver.values));
				//we->Insert(t, *(driver.values));
				//delete t;
				delete $3;
			} catch (std::runtime_error& e){
				if (t != NULL){
					delete t;
					t = NULL;					
				}		
				driver.error(e.what());				
				return Parser::SYNTAX_ERR;
			}
			delete driver.values; 
			driver.values = NULL;
		}
	;
	
values_or_query_spec:
		VALUES '(' insert_atom_commalist ')' {					
		}
	;
	
insert_atom_commalist:
		insert_atom {			
			driver.values = new std::vector<std::pair<std::string, std::string> >;		
			driver.values->push_back(std::pair<std::string, std::string>($1[0], $1[1]));
			delete[] $1;
		}
	|	insert_atom_commalist ',' insert_atom {
			driver.values->push_back(std::pair<std::string, std::string>($3[0], $3[1]));
			delete[] $3;
		}
	;
	
insert_atom:
		atom {
			$$ = $1;
		}
	|	NULLX {
			$$ = new std::string("NULL");
		}
	;

select_statement:
		SELECT opt_all_distinct selection table_exp {
			{
				Table* t1 = driver.table;
				WickyEngine* we = WickyEngine::getInstance();			
				try {
					driver.table = we->Select(t1, *(driver.getCondition()));
				} catch (std::runtime_error& e){
					driver.table = NULL;
					driver.error(e.what());
					return Parser::SYNTAX_ERR;
				}
				delete t1;
			}				
			if (driver.table == NULL) {				
				if (driver.cs != NULL) {
					delete driver.cs;
					driver.cs = NULL;
				}				
			} else 			
				try {
					if ($3) {
						Table* t1 = driver.table;
						WickyEngine* we = WickyEngine::getInstance();								
						driver.table = we->Project(t1, *(driver.cs));
						driver.table->printTable();
						delete driver.cs;					
						delete t1;
						driver.cs = NULL;
						t1 = NULL;
					} else {					
						driver.table->printTable();
					}
				} catch (std::runtime_error& e){
					driver.table = NULL;
					driver.error(e.what());
					return Parser::SYNTAX_ERR;
				}
							
		}
	;
	
opt_all_distinct:
		/* empty */
	|	ALL
	|	DISTINCT
	;
	
selection:
		scalar_exp_commalist { $$ = 1; }
	|	'*' { $$ = 0;}
	;
	
table_exp:
		from_clause opt_where_clause{			
		}
	;
	
scalar_exp_commalist:
		scalar_exp {
			driver.cs = new std::vector<std::pair<std::string, std::string> >;		
			driver.cs->push_back(std::pair<std::string, std::string>($1[0], $1[1]));
			delete[] $1;
		}
	|	scalar_exp_commalist ',' scalar_exp {
			driver.cs->push_back(std::pair<std::string, std::string>($3[0], $3[1]));
			delete[] $3;
		}
	;
	
opt_where_clause:
		/* empty */
	|	where_clause {

		}
	;
	
from_clause:
		FROM table_ref_commalist {			
		}
	;
	
table_ref_commalist:
		table_ref {
			WickyEngine* we = WickyEngine::getInstance();			
			try {				
				driver.table = we->GetTable(*$1);				
			} catch (std::runtime_error& e){
				driver.table = NULL;
				driver.error(e.what());
				return Parser::SYNTAX_ERR;
			}
		}
	|	table_ref_commalist ',' table_ref {
			WickyEngine* we = WickyEngine::getInstance();			
			try {
				Table *t1, *t2;
				t1 = driver.table;
				t2 = we->GetTable(*$3);
				driver.table = we->Join(t1, t2);
				delete t1;
				delete t2;
			} catch (std::runtime_error& e){
				driver.table = NULL;
				driver.error(e.what());
				return Parser::SYNTAX_ERR;
			}		
		}
	;
	
table_ref:
		table {			
			delete $1;
		}
	;

where_clause:
		WHERE search_condition {				
/*			std::vector< std::pair<std::string, std::string> > cond = driver.getCondition()->popCondition();
			
			std::vector< std::pair<std::string, std::string> >::iterator itr;
			for (itr = cond.begin(); itr != cond.end(); itr++){
				std::cout << itr->first << ":" << itr->second << std::endl;
			}*/
		}
	;
	
search_condition:	
	|	search_condition AND search_condition {		
	}
	|	predicate
	;

predicate:
		comparison_predicate
	;

comparison_predicate:
		scalar_exp COMPARISON scalar_exp {			
			driver.getCondition()->pushCondition($1[0], $1[1], *$2, $3[0], $3[1]);			
			delete[] $1;			
			delete $2;			
			delete[] $3;									
		}
	;
	
scalar_exp:
		atom { $$ = $1; }
	|	column_ref {
			$$ = $1; 
		}
	;

column_ref:
		NAME {			
			std::string* ret = new std::string[2];
			ret[0] = "COLUMN";
			ret[1] = *$1;			
			$$ = ret;
		}
	|	NAME '.' NAME	/* needs semantics */ {
			std::string* ret = new std::string[2];
			ret[0] = "COLUMN";
			ret[1] = *$1 + "." + *$3;			
			$$ = ret;
		}
	;
	
atom:
		literal { 
			$$ = $1;			
		}
	;
	
table:
		NAME { $$ = $1; }
	;
	
literal:
		STRING {			
			std::string* ret = new std::string[2];
			ret[0] = Schema::CHAR;
			ret[1] = *$1;			
			$$=ret;
			delete $1;
		}
	|	INTNUM {
			std::string* ret = new std::string[2];
			ret[0] = Schema::INT;
			ret[1] = *$1;
			$$=ret;
			delete $1;
		}
	|	APPROXNUM {
			std::string* ret = new std::string[2];
			ret[0] = Schema::FLOAT;
			ret[1] = *$1;
			$$=ret;
			delete $1;	
		}
	;	
	
column:
		NAME { 
			$$ = $1;			 
		}
	;
	
index:
		NAME { $$ = $1; }
	;
	
data_type:
		INT {
			$$ = new std::string[2];
			$$[0] = Schema::INT;
			std::stringstream ss;
			ss << Schema::INT_LENGTH;
			ss >> $$[1];
		}
	|	CHAR'(' INTNUM ')' {
			$$ = new std::string[2];
			$$[0] = Schema::CHAR;
			$$[1] = *$3;
			delete $3;
		}
	|	FLOAT {
			$$ = new std::string[2];
			$$[0] = Schema::FLOAT;
			std::stringstream ss;
			ss << Schema::FLOAT_LENGTH;
			ss >> $$[1];
		}
	;
	
%%

void
yy::SqlParser::error (const yy::SqlParser::location_type& l,
					   const std::string& m)
{
	driver.error (l, m);
}
